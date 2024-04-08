# `oqs-demos/h2load` benchmarks for Quantis QRNG Linux integration

This repository contains the scripts we used for running and analyzing benchmarks using Open Quantum Safe's [oqs-demos/h2load](https://github.com/open-quantum-safe/oqs-demos/tree/0.9.0/h2load).

## Building `oqs-demos/h2load`

Build the Docker image by running:

```bash
docker build -t h2load .
```

See more info about the build and usage in the [oqs-demos/h2load README](https://github.com/open-quantum-safe/oqs-demos/blob/0.9.0/h2load/README.md).

## Running Automated Benchmarks with `h2load-benchmarks.sh`

The `h2load-benchmarks.sh` script automates the process of running a number of benchmarks (here hardcoded to 100) with `oqs-demos/h2load`.

### Setting up

Set up a Python virtual environment and install the required dependencies. For example:

```bash
python3 -m venv .h2load-bench-venv
source .h2load-bench-venv/bin/activate
pip install --no-cache-dir -r requirements.txt
```

### Executing Benchmarks

Execute the benchmarks by running:

```bash
./h2load-benchmarks.sh -c <clients> -n <requests> -i <ip> -p <port> -m <mode>
```

Parameters:

- `<clients>`: Number of concurrent clients.
- `<requests>`: Number of requests per client.
- `<ip>`: IP address of the target server.
- `<port>`: Port on which the target server is listening.
- `<mode>`: Benchmark mode for annotation purposes, such as *no-qrng*, *qrng-raw-openssl*, *qrng-extraction-openssl*, *qrng-raw-rngtools*, *qrng-extraction-rngtools*.

### Outputs

The results of the benchmarks are organized in the following directory structure:

`./results/<clients>_clients_<requests>_requests/<mode>/`

For each execution, the script randomizes the sequence of algorithms and documents this order in the file located at `./results/<clients>_clients_<requests>_requests/<mode>/order_<run_number>.txt`. The results of the benchmarks are organized in the directory `./results/<clients>_clients_<requests>_requests/<mode>/run_<run_number>`. Within this directory, two subdirectories are created:

- `raw`: Contains raw output files, one for each algorithm, named `<algorithm>.txt`. These files capture the unprocessed benchmark results directly from the h2load tests.

- `processed`: This directory further branches into individual algorithm-named subdirectories. Each subdirectory houses two CSV files:
  - `summary_df.csv`: Provides a table with the summary statistics.
  - `timing_df.csv`: Provides a table with the detailed timing information.

We provide all the results in compressed form in the corresponding `.tar.gz` files.

## Data Analysis

In the `./data_analysis` directory, you'll find Jupyter notebooks and Python scripts for benchmark data analysis. The `data_analysis.ipynb` notebook contains the code we used for analyzing the data and generating the plots in our paper, with plots saved in `./data_analysis/plots`. For statistical analysis, the stats_tests.ipynb notebook performs Kolmogorov-Smirnov and Wilcoxon tests, with results located in `./data_analysis/results`.
