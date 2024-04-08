#!/bin/bash

CONTAINER_NAME="oqs-nginx-quantis"

while getopts "c:n:a:h:v:" opt; do
  case $opt in
    c) clients="$OPTARG" ;;
    n) requests="$OPTARG" ;;
    a) algorithm="$OPTARG" ;;
    v) version="$OPTARG" ;;
    *) echo 'Error in command line parsing' >&2
       exit 1
  esac
done

# Define the local path where you want to save the output.csv file. sav to results_clients_requests_mode folder
LOCAL_PATH="./results/$clients-c/$requests-r/$version/$algorithm"
CSV_TARGET_PATH="$LOCAL_PATH/output.csv"

mkdir -p $LOCAL_PATH

# Step 1: Execute read_shm inside the container. Ensure we're in the /opt/nginx directory.
docker exec -it $CONTAINER_NAME /bin/sh -c 'cd /opt/nginx && read_shm'

# Step 2: Copy the output.csv file from the container to your local machine. Assume output.csv is generated in /opt/nginx.
docker cp "$CONTAINER_NAME:/opt/nginx/output.csv" "$CSV_TARGET_PATH"

# Step 3: Delete the output.csv file inside the container, ensuring we're targeting the correct path.
docker exec -it $CONTAINER_NAME /bin/sh -c 'rm /opt/nginx/output.csv'

# Step 4: Delete the shared memory file inside the container
docker exec -it $CONTAINER_NAME /bin/sh -c 'rm /dev/shm/random_numbers_shm'

# Step 5: Call the Python script to aggregate the output.csv file.
python agg_measurements.py -c $clients -n $requests -a $algorithm -v $version -f $CSV_TARGET_PATH

echo "Script execution completed."