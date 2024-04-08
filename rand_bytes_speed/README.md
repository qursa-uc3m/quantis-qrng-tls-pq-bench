# rand_bytes_speed

This directory contains the scripts and programs used to measure the rate of random bytes generation from different sources with different methods.

## ``count_random.sh`

in the `./bash` directory run

```bash
./count_random.sh
```

## C

### `rand_bytes_speed_interval.c`

This program measures the average rate of random bytes generation over a given duration from character files like `/dev/random` (hardcoded, you can replace it by `/dev/qrandom` for the Quantis QRNG) and from the OpenSSL's `RAND_bytes` function.

You can build it with the following commands:


```bash
mkdir build
cd build
cmake  -DUSE_OPENSSL=ON/OFF ..
make
```

where `USE_OPENSSL` is a boolean variable that determines whether to use the OpenSSL library for generating random bytes. If you want `RAND_bytes` to get numbers from the Quantis QRNG, you have to ensure that you have installed the [Quantis QRNG OpenSSL Provider](https://github.com/qursa-uc3m/quantis-qrng-openssl-integration) and properly configured it trough the `openssl.cnf` file.

Run the program with the following command

```bash
./build/rand_bytes_speed_interval
```

### `rand_bytes_speed_vs_time.c`

This application measures the rate of random bytes generation at specified time intervals over a given sampling duration. The application has the ability to use three different sources for generating random bytes:

* 1. Character device files like `/dev/random`
* 2. The `getrandom` system call
* 3. The ID Quantique Quantis random number generator hardware devices

The results of the measurement are outputted in a CSV file which contains the step number and the rate of random bytes generation in megabytes per second.

#### Prerequisites

To use the Quantis hardware as a source of random bytes, you need to have the Quantis libraries installed in your system. The path to the Quantis libraries and apps should be provided as a CMake variable while building this project.

#### Building

The application uses CMake for building. To build the application, navigate to the project root directory and use the following commands:

```shell
mkdir build
cd build
cmake -DDEVICE=USB -DLIB_AND_APPS_PATH=/path/to/Quantis ..
make
```

#### usage

The DEVICE variable can be either `USB` or `PCIE`, depending on the type of your Quantis hardware. The `LIB_AND_APPS_PATH` should be the path to your Quantis libraries and apps.

* `-s`, --source: Source of random bytes
* `-t`, --timestep: Time interval in seconds at which the rate is measured
* `-d`, --duration: The total duration in seconds for which the program will run
* `-m`, --mode: The mode of operation. This can be one of file, getrandom, or quantis.
* `-r`, --readsize: The amount of data in bytes to read at each step. Default value is 1024 bytes.
* `-x, --xor_random: Enable XOR operation with random bytes. Set to "ON" to activate.

To use the /dev/random file as the source of random bytes:

```shell
./build/rand_bytes_speed_vs_time -s /dev/random -t 1 -d 60 -m file
```

To use the getrandom system call as the source of random bytes:

```shell
./build/rand_bytes_speed_vs_time -t 1 -d 60 -m getrandom
```

To use the Quantis hardware as the source of random bytes:

```shell
./build/rand_bytes_speed_vs_time -t 1 -d 60 -m quantis -x ON
```

## Rust

Go to the `./rust` directory and run

```bash
cargo build --release
```

(run `cargo run`for debug mode)

and then

```bash
./target/release/rand_bytes_speed_interval
```

```bash
./target/release/rand_bytes_speed_vs_time /dev/random 5 60
```

## Data analysis

Create the virtual environment and install the dependencies with

```bash
python3 -m venv .rand_bytes_speed-venv
source .rand_bytes_speed-venv/bin/activate
pip install --no-cache-dir -r requirements.txt
```
