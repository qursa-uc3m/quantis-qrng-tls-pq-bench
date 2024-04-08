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
