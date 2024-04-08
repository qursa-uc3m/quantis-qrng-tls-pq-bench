#!/bin/bash

SECONDS=0
BYTES_READ=0
BLOCK_SIZE=1024

while (( SECONDS <= 10 ))
do
    # Reads BLOCK_SIZE bytes from /dev/random and discard it
    dd if=/dev/random bs=$BLOCK_SIZE count=1 > /dev/null 2>&1
    BYTES_READ=$((BYTES_READ + BLOCK_SIZE))
done

echo "Average rate: $(((BYTES_READ / SECONDS) / 1024 )) kB/sec"