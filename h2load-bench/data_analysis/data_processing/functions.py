import os
import pandas as pd
import numpy as np
from scipy.stats import t as t_dist
import matplotlib.colors as mcolors


def convert_to_mbps(value):
    if value.endswith("KB/s"):
        return round(float(value.replace("KB/s", "")) / 1024, 2)
    else:
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

def xor_columns_ratios(input_df, error_column="Req/s_error"):
    results = []
    to_ratio_column = "Req/s_mean"
    for alg, alg_group in input_df.groupby("algorithm"):
        base_modes = [mode for mode in alg_group["mode"].unique() if "-xor" not in mode]

        for base_mode in base_modes:
            if f"{base_mode}-xor" in alg_group["mode"].values:
                base_row = alg_group[alg_group["mode"] == base_mode].iloc[0]
                xor_row = alg_group[alg_group["mode"] == f"{base_mode}-xor"].iloc[0]

                ratio = base_row[to_ratio_column] / xor_row[to_ratio_column]
                error = ratio * ((xor_row[error_column] / xor_row[to_ratio_column])**2 + 
                                 (base_row[error_column] / base_row[to_ratio_column])**2)**0.5

                results.append({"algorithm": alg, "mode": base_mode, "xor_ratio": ratio, "xor_error": error})
    output_df = pd.DataFrame(results)

    return output_df

def combine_summary_dataframes(n_clients, n_requests, modes_list, n_runs=10, is_review=False):
    run_dfs = []
    summary_dfs = []

    for mode in modes_list:
        base_dir_name = f"{n_clients}_clients_{n_requests}_requests"
        if not os.path.isdir(os.path.join("..", "results", base_dir_name)):
            raise Exception(f"Directory {base_dir_name} does not exist")
        dir_name = f"{base_dir_name}/{mode}"
        if is_review:
            dir_name = f"{dir_name}_review"
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

def get_color(color_palette, mode_index, j, num_data_types):
    if j == num_data_types - 1:
        return color_palette[mode_index % len(color_palette)]
    color = color_palette[mode_index % len(color_palette)]
    alpha = 0.8
    delta = 0.125 * j
    modified_color = tuple(component + delta for component in mcolors.to_rgb(color))
    return (*modified_color, alpha)

def compute_error(df, n, std_column=None, confidence_level = 0.95):
    """
    Compute the error using t-student for a given dataframe and number of runs.
    """
    if std_column is None:
        raise Exception("std_column must be specified")
    
    sem = df[std_column] / np.sqrt(n)
    return sem.apply(lambda x: compute_t_error(x, n, confidence_level=0.95))

def propagate_t_error_for_avg(ses):
    """
    Propagate the error for the mean from individual standard errors.
    """
    return np.sqrt(np.sum(ses**2)) / len(ses)

def get_pooled_sd(df, mean_column, sd_column):
    """
    Compute the pooled standard deviation for a given dataframe.
    """
    mean_of_variances = np.mean(df[sd_column] ** 2)
    variance_of_means = np.var(df[mean_column])
    pooled_variance = mean_of_variances + variance_of_means
    pooled_sd = np.sqrt(pooled_variance)
    return pooled_sd

def get_stats(df, n_clients):
    n_sample = len(df)
    output = {}
    if 'Mean' not in df or 'SD' not in df:
        mean_column = "Mean (s)"
        sd_column = "SD (s)"
    else:
        mean_column = "Mean"
        sd_column = "SD"

    individual_t_errors = compute_error(df, n_clients, std_column=sd_column)
    propagated_t_error = propagate_t_error_for_avg(individual_t_errors)

    if (df[sd_column] == 0).any():
        epsilon = 1e-10
        df[sd_column] = df[sd_column].replace(0, epsilon)

    output['weighted_mean'] = np.average(df[mean_column], weights=1 / (df[sd_column] ** 2))
    output['weighted_sd'] = np.sqrt(1 / np.sum(1 / (df[sd_column] ** 2)))
    output['weighted_t_error'] = compute_t_error(output['weighted_sd']/np.sqrt(n_sample), n_sample, confidence_level=0.95)
    output['avg_of_means'] = np.mean(df[mean_column])
    output['propagated_t_error'] = propagated_t_error
    output['pooled_sd'] = get_pooled_sd(df, mean_column, sd_column)
    output['pooled_t_error'] = compute_t_error(output['pooled_sd']/np.sqrt(n_sample*n_clients), n_sample*n_clients, confidence_level=0.95)

    return pd.Series(output, index=['weighted_mean', 'weighted_sd', 'weighted_t_error', 'avg_of_means', 'propagated_t_error', 'pooled_sd', 'pooled_t_error'])

def combine_timing_dataframes(n_clients, n_requests, modes_list, n_runs=10, error_calculation_method=None, is_review=False):

    timing_dfs = []

    for mode in modes_list:
        base_dir_name = f"{n_clients}_clients_{n_requests}_requests/{mode}"
        if is_review:
            base_dir_name = f"{base_dir_name}_review"
        dir_name = f"{base_dir_name}-{n_runs}runs"
        if not os.path.isdir(os.path.join("..", "results", dir_name)):
            continue

        runs_dirs = [d for d in os.listdir(os.path.join("..", "results", dir_name)) if "run_" in d]
        
        for run_dir in runs_dirs:
            processed_dir_path = os.path.join("..", "results", dir_name, run_dir, "processed")
            for algorithm in os.listdir(processed_dir_path):
                timing_csv_path = os.path.join(processed_dir_path, algorithm, "timing_df.csv")
                if os.path.isfile(timing_csv_path):
                    timing_df = pd.read_csv(timing_csv_path)
                    timing_df["algorithm"] = algorithm
                    timing_df["mode"] = mode
                    timing_dfs.append(timing_df)
    timing_df_all = pd.concat(timing_dfs, ignore_index=True)

    timing_df_all_reqs = timing_df_all[timing_df_all["index"] == "req/s"]
    timing_df_all = timing_df_all[timing_df_all["index"] != "req/s"]

    for column in ['Min', 'Max', 'Mean', 'SD']:
        timing_df_all[column] = timing_df_all[column].apply(convert_to_s)
        pass
    timing_df_all = timing_df_all.rename(columns={
        'Min': 'Min (s)',
        'Max': 'Max (s)',
        'Mean': 'Mean (s)',
        'SD': 'SD (s)'
    })
    timing_df_all_reqs['Mean'] = timing_df_all_reqs['Mean'].astype(float)
    timing_df_all_reqs['SD'] = timing_df_all_reqs['SD'].astype(float)
    # Calculating weighted mean and weighted SD for timing_df_all_reqs
    grouped_data_reqs = timing_df_all_reqs.groupby(['index', 'algorithm', 'mode'])
    timing_df_all_reqs = grouped_data_reqs.apply(lambda x: error_calculation_method(x, n_clients)).reset_index()

    # Calculating aggregated Min and Max for timing_df_all_reqs
    aggregated_min_max_reqs = grouped_data_reqs.agg({'Min': 'min', 'Max': 'max'}).reset_index()
    timing_df_all_reqs = pd.merge(timing_df_all_reqs, aggregated_min_max_reqs, how='left', on=['index', 'algorithm', 'mode'], suffixes=('', '_agg'))

    # Calculating weighted mean and weighted SD for timing_df_all
    grouped_data_all = timing_df_all.groupby(['index', 'algorithm', 'mode'])
    timing_df_all = grouped_data_all.apply(lambda x: error_calculation_method(x, n_clients)).reset_index()

    # Calculating aggregated Min and Max for timing_df_all
    aggregated_min_max_all = grouped_data_all.agg({'Min (s)': 'min', 'Max (s)': 'max'}).reset_index()
    timing_df_all = pd.merge(timing_df_all, aggregated_min_max_all, how='left', on=['index', 'algorithm', 'mode'], suffixes=('', '_agg'))

    return timing_df_all, timing_df_all_reqs

def compute_ratios(summary_df_all, error_column="Req/s_error", confidence_level=0.95):

    no_qrng_data = summary_df_all[summary_df_all["mode"] == "NoQRNG"]["Req/s_mean"]
    no_qrng_std = summary_df_all[summary_df_all["mode"] == "NoQRNG"][error_column]

    ratios = []
    errors = []

    modes = summary_df_all["mode"].unique()
    modes = [mode for mode in modes if mode != "NoQRNG"]

    # Compute the ratio and error for each mode
    for mode in modes:
        mode_data = summary_df_all[summary_df_all["mode"] == mode]["Req/s_mean"]
        mode_std = summary_df_all[summary_df_all["mode"] == mode][error_column]
        
        paired_data = zip(mode_data, no_qrng_data, mode_std, no_qrng_std)
        
        relative_diffs = []
        relative_errors = []
        
        for value_1, value_2, std_1, std_2 in paired_data:
            if value_1 != 0:
                ratio = value_2 / value_1
                error_ratio = ratio * ((std_1 / value_1) ** 2 + (std_2 / value_2) ** 2) ** 0.5
                
                relative_diffs.append(ratio)
                relative_errors.append(error_ratio)

        # Compute the t-value for the given confidence level and degrees of freedom
        t_value = t_dist.ppf((1 + confidence_level) / 2, len(relative_diffs) - 1)
        # Take the average of all relative differences and their errors
        average_ratio = sum(relative_diffs) / len(relative_diffs) if relative_diffs else 0
        error_of_average = (sum([error**2 for error in relative_errors]) / len(relative_errors))**0.5 if relative_errors else 0
        final_error = t_value * error_of_average
        
        ratios.append((mode, average_ratio, final_error))

    ratio_df = pd.DataFrame(ratios, columns=["mode", "Average Ratio", "Error"])

    ratio_df["Average Ratio"] = ratio_df["Average Ratio"].apply(lambda x: "{:.2f}".format(x))
    ratio_df["Error"] = ratio_df["Error"].apply(lambda x: "{:.2f}".format(x))

    return ratio_df

def create_table(ratio_df):

    headers = ["\\textbf{Mode}", "\\textbf{Average Ratio of Req/s}", "\\textbf{Error}"]
    headers_line = " & ".join(headers) + " \\\\ \\hline"
    latex_table = ratio_df.to_latex(index=False, column_format='|c|c|c|', escape=False, header=False)
    latex_table = latex_table.splitlines()
    latex_table.insert(1, headers_line)
    latex_table = "\\hline\n".join(latex_table[:-1]) + "\n" + latex_table[-1]

    caption = "\\caption{Ratios of Req/s between QRNG integration modes with respect to \\textit{no-qrng} mode}\n"
    label = "\\label{tab:ratios}\n"

    latex_table = '\\begin{table}[h]\n\\centering\n' + caption + label + latex_table + '\\end{table}'

    latex_table = latex_table.replace("qrng-extraction-", "")
    latex_table = latex_table.replace("qrandom-xor", '\\textbf{qrandom-xor}').replace("rngtools_reseed1", '\\textbf{rngtools\_reseed1}')

    return latex_table

if __name__=="__main__":
    print("Data processing functions")