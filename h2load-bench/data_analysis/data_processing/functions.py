import os
import pandas as pd
import numpy as np
from scipy.stats import t as t_dist


def convert_to_mbps(value):
    if value.endswith("KB/s"):
        # convert KB/s to MB/s (1 MB = 1024 KB)
        return round(float(value.replace("KB/s", "")) / 1024, 2)
    else:
        # remove "MB/s" from the value
        return round(float(value.replace("MB/s", "")), 2)
    
def convert_to_ms(value):
    if isinstance(value, str):
        if value.endswith('us'):
            return float(value.rstrip('us')) / 1000
        elif value.endswith('ms'):
            return float(value.rstrip('ms'))
        elif value.endswith('s'):
            return float(value.rstrip('s')) * 1000
    else:
        return np.nan
    
def convert_to_s(value):
    if isinstance(value, str):
        if value.endswith('us'):
            return float(value.rstrip('us')) / (1000*1000)
        elif value.endswith('ms'):
            return float(value.rstrip('ms')) / 1000
        elif value.endswith('s'):
            return float(value.rstrip('s'))
    else:
        return np.nan
    
def compute_t_error(sem, n, confidence_level=0.95):
    """
    Compute the t-student error for the mean.
    """
    alpha = 1 - confidence_level
    t_value = t_dist.ppf(1 - alpha/2, n-1)
    return t_value * sem

def compute_error(df, n, std_column=None, confidence_level = 0.95):
    """
    Compute the error using t-student for a given dataframe and number of runs.
    """
    if std_column is None:
        raise Exception("std_column must be specified")
    
    sem = df[std_column] / np.sqrt(n)
    return sem.apply(lambda x: compute_t_error(x, n, confidence_level=0.95))


def combine_summary_dataframes(n_clients, n_requests, modes_list, n_runs=10):
    run_dfs = []
    summary_dfs = []

    for mode in modes_list:
        base_dir_name = f"{n_clients}_clients_{n_requests}_requests"
        if not os.path.isdir(os.path.join("..", "results", base_dir_name)):
            raise Exception(f"Directory {base_dir_name} does not exist")
        dir_name = f"{base_dir_name}/{mode}"
        dfs = []
        runs_dirs = [d for d in os.listdir(os.path.join("..", "results", f"{dir_name}-{n_runs}runs")) if "run_" in d]
        for run_dir in runs_dirs:
            processed_dir_path = os.path.join("..", "results", f"{dir_name}-{n_runs}runs", run_dir, "processed")
            for algorithm in os.listdir(processed_dir_path):
                summary_csv_path = os.path.join(processed_dir_path, algorithm, "summary_df.csv")
                if os.path.isfile(summary_csv_path):
                    summary_df = pd.read_csv(summary_csv_path)
                    summary_df.drop(columns=["Finished in (ms)", "traffic_total", "traffic_headers", "traffic_data"], errors='ignore', inplace=True)
                    summary_df["algorithm"] = algorithm
                    summary_df["mode"] = mode
                    summary_df["data/s"] = summary_df["data/s"].apply(convert_to_mbps)
                    summary_df.rename(columns={"data/s": "data/s_MB/s"}, inplace=True)
                    dfs.append(summary_df)

        # Concatenate dataframes from all runs and compute mean and standard deviation
        run_df = pd.concat(dfs, ignore_index=True)
        mean_df = run_df.groupby(["algorithm", "mode"]).mean().reset_index()
        std_df = run_df.groupby(["algorithm", "mode"]).std().reset_index()

        mean_df.rename(columns={col: f"{col}_mean" for col in mean_df.columns if col not in ["algorithm", "mode"]}, inplace=True)
        std_df.rename(columns={col: f"{col}_std" for col in std_df.columns if col not in ["algorithm", "mode"]}, inplace=True)
        summary_df = pd.merge(mean_df, std_df, on=["algorithm", "mode"])
        summary_df["Req/s_error"] = compute_error(summary_df, n_runs, std_column="Req/s_std")

        summary_dfs.append(summary_df)
        run_dfs.append(run_df)

    run_df_all = pd.concat(run_dfs, ignore_index=True)
    summary_df_all = pd.concat(summary_dfs, ignore_index=True)
    return run_df_all, summary_df_all

if __name__=="__main__":
    print("Data processing functions")