# `oqs-demos/nginx` with Quantis QRNG OpenSSL Integration

This repository is a fork of [oqs-demos/nginx](https://github.com/open-quantum-safe/oqs-demos/tree/0.9.0/nginx) with added support for the Quantis QRNG OpenSSL provider, enabling nginx to use the Quantis QRNG for random number generation.

## Prerequisites

Before using this repository, ensure you have the Quantis QRNG devices and/or drivers properly installed on your system. For installation instructions and more details, visit [Quantis Software](https://www.idquantique.com/random-number-generation/products/quantis-software/).

## Building

### Setting up the Quantis QRNG OpenSSL Provider

After cloning this repository, you have to initialize the submodule `quantis-qrng-openssl-integration` with the provider:

```bash
git submodule update --init
```

### Building the Docker Image

To build the Docker image, execute the following command:

```bash
docker build \
    --build-arg QUANTIS_QRNG=true \
    --build-arg QRNG_DEVICE=USB \
    --build-arg QUANTIS_LIB=YES \
    --build-arg DEVICE_NUMBER=0 \
    --build-arg XOR_RANDOM=ON \
    --build-arg MEASURE_RNG=ON \
    --build-arg QRNG_DEBUG=ON \
    -t oqs-nginx-quantis .
```

where the arguments are as follows:

- `QRNG_DEVICE`: The QRNG device type. Default is USB.
- `QRNG_DEBUG`: Enables or disables QRNG debugging. Default is ON.
- `QUANTIS_QRNG`: Activates the Quantis QRNG. Default is true.
- `QUANTIS_LIB`: Activates the Quantis library. Default is YES. If set to NO, `/dev/qrandom{DEVICE_NUMBER}` is used instead of the Quantis library.
- `DEVICE_NUMBER`: The device number. Default is 0.
- `XOR_RANDOM`: Enables or disables XORing the QRNG random bytes with getrandom(). Default is ON.
- `MEASURE_RNG`: Registers the amount of random bytes requested to the provider in a `/random_numbers_shm` shared memory. Default is OFF.

Note: The Docker image uses the nginx configuration specified in the `./nginx-conf/nginx.conf` file.

## Runing with Quantis QRNG

To enable access to the host Quantis QRNG device, add the `--priviliged` flag to the `docker run` command:

```bash
docker run --privileged --detach --rm --env-file env.list --name oqs-nginx-quantis -p 4433:4433 oqs-nginx-quantis
```

The `--env-file env.list` flag sets environment variables from the file `env.list` that contains the KEMS used by the nginx server, e.g.,

```bash
DEFAULT_GROUPS=P-256:P-384:P-521:X448:X25519:bikel1:bikel3:kyber512:kyber768:kyber1024:frodo640aes:frodo640shake:frodo976aes:frodo976shake:frodo1344aes:frodo1344shake:hqc128:hqc192:p256_bikel1:p384_bikel3:p256_kyber512:p384_kyber768:p521_kyber1024:p256_frodo640aes:p256_frodo640shake:p384_frodo976aes:p384_frodo976shake:p521_frodo1344aes:p521_frodo1344shake
```

By default, if `env.list` is not specified, the server will use the following KEMs

```bash
DEFAULT_GROUPS=x25519:x448:kyber512:p256_kyber512:kyber768:p384_kyber768:kyber1024:p521_kyber1024
```

To stop the running container, execute:

```bash
docker stop oqs-nginx-quantis
```

## Registering the amount of random bytes requested

Build the image with the `MEASURE_RNG` argument set to `ON`, for example:

```bash
docker build --no-cache --build-arg QUANTIS_QRNG=true \
    --build-arg QRNG_DEVICE=USB \
    --build-arg QUANTIS_LIB=YES \
    --build-arg DEVICE_NUMBER=0 \
    --build-arg XOR_RANDOM=ON  \
    --build-arg QRNG_DEBUG=ON \
    --build-arg MEASURE_RNG=ON \
    -t oqs-nginx-quantis .
```

Then, with the container running, you can access the shared memory by running

```bash
docker exec -it oqs-nginx-quantis /bin/bash
```

and then

```bash
read_shm
```

If you want to clear the shared memory, which is `/random_numbers_shm` run

```bash
rm /dev/shm/random_numbers_shm
```

### Automated measurement retrieval and aggregation

Create a Python virtual environment and install the requirements:

```bash
conda create -n measure_rand_bytes-venv python=3.10
conda activate measure_rand_bytes-venv
pip install --no-cache-dir -r ./measyre_rand_bytes/requirements.txt
```

We have a script that retrieve the measurements from inside the container and aggregate them. But there is no need to run it, later we will call a wrapper that also launches `h2load`. This is how the script for the server works:

```bash
./measure_container.sh -c 1 -n 1 -a kyber512 -v 2
```

where

- `-c` is the number of clientes making requests
- `-n` is the number of requests per client
- `-a` is the algorithm used
- `-v` is the http version used, valid values are 1_1 and 2

Ensure that the `oqs-nginx-quantis` is up and running

```bash
docker build --no-cache --build-arg QUANTIS_QRNG=true     --build-arg QRNG_DEVICE=USB     --build-arg QUANTIS_LIB=NO     --build-arg DEVICE_NUMBER=0     --build-arg XOR_RANDOM=OFF      --build-arg QRNG_DEBUG=ON     --build-arg MEASURE_RNG=ON     -t oqs-nginx-quantis .
docker run --privileged --detach --rm --env-file env.list --name oqs-nginx-quantis -p 4433:4433 oqs-nginx-quantis
```

Then, run the script to retrieve the measurements:

```bash
./measure_container.sh -c 1 -n 1 -a kyber512 -v 2
```

where

- `-c` is the number of clientes making requests
- `-n` is the number of requests per client
- `-a` is the algorithm used
- `-v` is the http version used, valid values are 1_1 and 2

Now we need to build the `h2load` image as specified in the `h2load-bench` directory. After that, we can run the wrapper script that launches `h2load` and the script to retrieve the measurements:

```bash
./measurements_wrapper.sh -c 1 -n 1 -p <port> -v 2 -a kyber512
```

where the parameters are the same as before. If the `-a` parameter is not passed, all the algorithms will be used.