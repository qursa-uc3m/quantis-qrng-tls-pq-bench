import argparse
import pandas as pd
import os

TARGET_DIR = './results/aggregated'
TARGET_FILE = os.path.join(TARGET_DIR, 'measurements.csv')
COLUMNS = ['clients', 'requests', 'algorithm', 'http_version', 'measured_bytes']

def get_measured_bytes(file_path):

    with open(file_path, 'r') as f:
        measured_bytes = f.read()

    try:
        measured_bytes = int(measured_bytes)
    except ValueError:
        raise ValueError("Error: cannot convert measured bytes to an integer.")
    return measured_bytes


def check_and_update_csv(sample):

    # Check if the file exists
    if not os.path.exists(TARGET_DIR):
        os.makedirs(TARGET_DIR)
    # Check if the file exists
    if not os.path.isfile(TARGET_FILE):
        # Create a new DataFrame and save it if the file does not exist
        df = pd.DataFrame(columns=COLUMNS)
        df.loc[0] = sample
    else:
        # Load the existing data
        df = pd.read_csv(TARGET_FILE)
        
        # Check for duplicate rows
        duplicates = df[(df['clients'] == sample[0]) & 
                        (df['requests'] == sample[1]) & 
                        (df['algorithm'] == sample[2]) & 
                        (df['http_version'] == sample[3])]
        
        if not duplicates.empty:
            raise ValueError("Record already exists.")
        
        # Append new data to the DataFrame
        new_row = pd.DataFrame([sample], columns=COLUMNS)
        df = pd.concat([df, new_row], ignore_index=True)
    
    # Save the updated DataFrame to the CSV
    df.to_csv(TARGET_FILE, index=False)
    print("CSV file has been updated.")

def main(args):
    # Assuming measured_bytes is to be manually entered or calculated
    measured_bytes = get_measured_bytes(args.file)

    # New data to be added
    sample = [args.clients, args.requests, args.algorithm, args.version, measured_bytes]

    # Check and update the CSV file
    check_and_update_csv(sample)

if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Process measurement data and update CSV.")
    parser.add_argument('-c', '--clients', type=int, required=True, help='Number of clients')
    parser.add_argument('-n', '--requests', type=int, required=True, help='Number of requests')
    parser.add_argument('-a', '--algorithm', required=True, help='Algorithm used')
    parser.add_argument('-v', '--version', required=True, help='HTTP version')
    parser.add_argument('-f', '--file', required=True, help='Path to the CSV file to process')

    args = parser.parse_args()
    main(args)