#!/bin/bash
# usage: ./load:_driver.sh mode qrn_num
# Example:
#   RNG mode, num auto: ./load:_driver.sh 0 0
#   SAMPLE mode, 12 chips: ./load:_driver.sh 1 12

# Remove the existing kernel module

set -eu

rmmod quantis_chip_pcie 2>/dev/null || true

echo "Loading driver...."
echo "mode=$1"
echo "num=$2"

if ! insmod ./driver/quantis_chip_pcie.ko default_qrng_mode=$1 default_qrng_num=$2; then
  echo "Error: Kernel module did not load properly."
  echo "FAILED"
  exit 1
fi

echo "DONE"
