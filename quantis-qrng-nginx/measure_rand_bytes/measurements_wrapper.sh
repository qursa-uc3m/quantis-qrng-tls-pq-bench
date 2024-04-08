#!/bin/bash

ip="127.0.0.1"
port="4433"

while getopts ":c:n:i:p:a:v:" opt; do
  case ${opt} in
    c ) clients=$OPTARG ;;
    n ) requests=$OPTARG ;;
    i ) ip=$OPTARG ;;
    p ) port=$OPTARG ;;
    a ) algorithm=$OPTARG ;;
    v ) version=$OPTARG ;;
    \? ) echo "Usage: cmd [-c] [-n] [-i] [-p] [-a] [-v]"
         exit 1 ;;
  esac
done

if [ -z "$clients" ] || [ -z "$requests" ] || [ -z "$version" ]; then
    echo "All parameters (-c, -n, -v) must be provided."
    exit 1
fi
if [ "$version" == "1_1" ]; then
    http_version="http/1.1"
elif [ "$version" == "2" ]; then
    http_version="h2"
else
    echo "Invalid version. Please use 1_1 or 2."
    exit 1
fi

echo "Measuring random bytes required by nginx for $clients clients and $requests requests using HTTP/$version."
echo "IP: $ip, Port: $port"
echo "Ensure the nginx container is up and running."
echo "Ensure the h2load container is built and is called h2load."

# Define all algorithms if none is specified
if [ -z "$algorithm" ]; then
    algorithms=("P-256" "P-384" "P-521" "X448" "X25519" "kyber512" "kyber768" "kyber1024" "p256_kyber512" "p384_kyber768" "p521_kyber1024")
    echo "Running for all algorithms."
else
    algorithms=("$algorithm")
    echo "Running for algorithm: $algorithm"
fi

echo "--------------------------------------------------------------------------------"

for current_algorithm in "${algorithms[@]}"; do
    echo "Running for algorithm: $current_algorithm ....."
    
    # Run the h2load client command
    docker run --rm --network=host --name h2load -it h2load h2load -n "$requests" -c "$clients" --alpn-list="$http_version" "https://$ip:$port" --groups "$current_algorithm"

    # Now, retrieve the measurement from the nginx container
    ./measure_container.sh -c "$clients" -n "$requests" -a "$current_algorithm" -v "$version"
done

echo "Measurement process completed."
