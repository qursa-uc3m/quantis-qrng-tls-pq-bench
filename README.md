# Quantis QRNG Linux Integration h2load Benchmarking with Post-Quantum Cryptography

This repository provides benchmarking tools for evaluating the performance of an **nginx** HTTPS server configured to use OpenSSL with the Quantis QRNG OpenSSL provider ([qursa-uc3m/quantis-qrng-openssl-integration](https://github.com/qursa-uc3m/quantis-qrng-openssl-integration)) for random number generation for TLS. It usess **h2load** to benchmark the nginx server, exploring the combination of post-quantum cryptography and quantum random number generators.

## Prerequisites

Before using this repository, ensure you have the Quantis QRNG devices and/or drivers properly installed on your system. For installation instructions and more details, visit [Quantis Software](https://www.idquantique.com/random-number-generation/products/quantis-software/).

## Repository Structure

### Nginx Server with Quantis QRNG OpenSSL Provider

The `quantis-qrng-nginx` directory houses a Docker image derived from [oqs-demos/nginx](https://github.com/open-quantum-safe/oqs-demos/tree/0.9.0/nginx). This image is extended to integrate with the Quantis QRNG OpenSSL provider, which allows nginx to leverage quantum-generated entropy for secure random number generation.

For more detailed instructions and configuration options, please refer to the `README.md` file located in the `quantis-qrng-nginx` directory.

### h2load Benchmarking

Scripts for conducting and analyzing benchmarks are located in the `h2load-bench` directory. These benchmarks utilize [oqs-demos/h2load](https://github.com/open-quantum-safe/oqs-demos/tree/0.9.0/h2load) from [Open Quantum Safe](https://openquantumsafe.org/). The results from the paper are provided in a compressed format within the respective directories as specified in the corresponding `README.md` files in the h2load-bench directory.

For more detailed instructions see the `README.md` file in the `h2load-bench` directory.
