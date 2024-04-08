#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <getopt.h>
#include <sys/random.h>
#include "Quantis.h"

#define RESULT_DIR "./results"

// Quantis QRNG hardware device parameters:
#ifdef DEVICE_USB
    QuantisDeviceType deviceType = QUANTIS_DEVICE_USB;
#elif defined(DEVICE_PCIE)
    QuantisDeviceType deviceType = QUANTIS_DEVICE_PCI;
#endif
int cardNumber = 0;
double UNITS = 1024 * 1024;
int READ_SIZE = 4096;
double total_time = 0;
double total_reciprocal_of_rate = 0;

int get_from_character_device(char *source, double *mbPerSec, double *times, int step_duration, int total_duration, bool XOR_RANDOM);
int get_from_getrandom(double *mbPerSec, double *times, int step_duration, int total_duration);
int get_from_quantis(QuantisDeviceHandle *handle, double *mbPerSec, double *times, int step_duration, int total_duration, bool XOR_RANDOM);

int parse_arguments(int argc, char *argv[], char **source, int *step_duration, int *total_duration, char **mode, int *read_size, bool *XOR_RANDOM) {
    int opt;
    *XOR_RANDOM = false;
    while ((opt = getopt(argc, argv, "s:t:d:m:r:x:")) != -1) {
        switch (opt) {
            case 's':
                *source = optarg;
                break;
            case 't':
                *step_duration = atoi(optarg);
                break;
            case 'd':
                *total_duration = atoi(optarg);
                break;
            case 'm':
                *mode = optarg;
                break;
            case 'r':
                *read_size = atoi(optarg);
                break;
            case 'x':
                *XOR_RANDOM = (strcmp(optarg, "ON") == 0);
                break;
            default:
                fprintf(stderr, "Usage: %s [-s source] [-r read_size] -t step_duration -d total_duration -m mode [-x xor_random]\n", argv[0]);
                return 1;
        }
    }

    if (*mode == NULL || (*source == NULL && strcmp(*mode, "getrandom") != 0 && strcmp(*mode, "quantis") != 0) || *step_duration == -1 || *total_duration == -1) {
        fprintf(stderr, "Usage: %s [-s source] [-r read_size] -t step_duration -d total_duration -m mode [-x xor_random]\n", argv[0]);
        return 1;
    }

    // Set the global READ_SIZE to the value of read_size if provided, or to 1024 if not
    READ_SIZE = (*read_size > 0) ? *read_size : READ_SIZE;

    printf("source: %s\n", *source);
    printf("step_duration: %d\n", *step_duration);
    printf("total_duration: %d\n", *total_duration);
    printf("mode: %s\n", *mode);
    printf("read_size: %d\n", READ_SIZE);
    printf("xor_random: %s\n", *XOR_RANDOM ? "ON" : "OFF");

    return 0;
}

int create_csv_file(int *step_duration, int steps, double *mbPerSec, double *times) {

    char mkdir_cmd[256];
    snprintf(mkdir_cmd, sizeof(mkdir_cmd), "mkdir -p %s", RESULT_DIR);
    if (system(mkdir_cmd) == -1) {
        perror("Error creating directory");
        return 1;
    }

    char csv_file_name[256];
    time_t t = time(NULL);
    struct tm *tm_info = localtime(&t);
    strftime(csv_file_name, sizeof(csv_file_name), RESULT_DIR "/results_%Y_%m_%d_%H_%M_%S.csv", tm_info);
    FILE *csv_file = fopen(csv_file_name, "w");

    if (csv_file == NULL) {
        perror("Could not open CSV file");
        return 1;
    }

    fprintf(csv_file, "step,rate\n");
    for (int i = 0; i < steps; i++) {
        fprintf(csv_file, "%.7f,%.7f\n", times[i], mbPerSec[i]);
    }

    fclose(csv_file);

    printf("Results saved to %s\n", csv_file_name);

    return 0;
}

int get_from_character_device(char *source, double *mbPerSec, double *times, int step_duration, int total_duration, bool XOR_RANDOM) {
    unsigned char buffer[READ_SIZE];
    unsigned char tmp_buffer[READ_SIZE];
    ssize_t bytesRead;
    size_t totalBytesRead = 0;
    int step_count = 0;

    int randomData = open(source, O_RDONLY);
    if (randomData < 0) {
        perror("Could not open source");
        return 1;
    }

    struct timespec begin_time, readEnd;
    clock_gettime(CLOCK_MONOTONIC, &begin_time);

    while (true) {
        bytesRead = read(randomData, buffer, READ_SIZE);
        // XOR the bytes from Quantis with bytes from getrandom
        if (XOR_RANDOM) {
            ssize_t n = getrandom(tmp_buffer, READ_SIZE, 0);

            if (n != READ_SIZE) {
                fprintf(stderr, "Error: read only %zd bytes out of %d\n", n, READ_SIZE);
                return 1;
            }

            for (int i = 0; i < READ_SIZE; i++) {
                buffer[i] ^= tmp_buffer[i];
            }
            memset(tmp_buffer, 0, sizeof(tmp_buffer));
        }
        
        clock_gettime(CLOCK_MONOTONIC, &readEnd);

        if (bytesRead < 0) {
            perror("Error reading source");
            return 1;
        }
        
        totalBytesRead += bytesRead;
        double elapsed_time = (readEnd.tv_sec - begin_time.tv_sec) + 1e-9 * (readEnd.tv_nsec - begin_time.tv_nsec);

        if (elapsed_time >= (step_count + 1) * step_duration) {
            mbPerSec[step_count] = (totalBytesRead / elapsed_time) / UNITS;
            times[step_count] = elapsed_time;
            step_count++;
        }

        if (elapsed_time >= total_duration) {
            break;
        }
    }
    close(randomData);

    return 0;
}

int get_from_getrandom(double *mbPerSec, double *times, int step_duration, int total_duration) {
    unsigned char buffer[READ_SIZE];
    ssize_t bytesRead;
    size_t totalBytesRead = 0;
    int step_count = 0;

    struct timespec begin_time, readEnd;
    clock_gettime(CLOCK_MONOTONIC, &begin_time);

    while (true) {
        bytesRead = getrandom(buffer, READ_SIZE, 0);
        clock_gettime(CLOCK_MONOTONIC, &readEnd);

        if (bytesRead < 0) {
            perror("Error reading source");
            return 1;
        }
        totalBytesRead += bytesRead;
        double elapsed_time = (readEnd.tv_sec - begin_time.tv_sec) + 1e-9 * (readEnd.tv_nsec - begin_time.tv_nsec);

        if (elapsed_time >= (step_count + 1) * step_duration) {
            mbPerSec[step_count] = (totalBytesRead / elapsed_time) / UNITS;
            times[step_count] = elapsed_time;
            step_count++;
        }

        if (elapsed_time >= total_duration) {
            break;
        }
    }

    return 0;
}

int open_quantis(QuantisDeviceHandle **handle) {
    int status = QuantisOpen(deviceType, cardNumber, handle);
    if (status != QUANTIS_SUCCESS) {
        fprintf(stderr, "QuantisOpen failed with error: %s\n", QuantisStrError(status));
        return 1;
    }
    return 0;
}

void close_quantis(QuantisDeviceHandle *handle) {
    if (handle != NULL) {
        QuantisClose(handle);
    }
}

int get_from_quantis(QuantisDeviceHandle *handle, double *mbPerSec, double *times, int step_duration, int total_duration, bool XOR_RANDOM) {
    unsigned char buffer[READ_SIZE];
    unsigned char tmp_buffer[READ_SIZE];
    int bytesRead;
    size_t totalBytesRead = 0;
    int step_count = 0;
    
    struct timespec begin_time, readEnd;
    clock_gettime(CLOCK_MONOTONIC, &begin_time);

    while (true) {
        bytesRead = QuantisReadHandled(handle, buffer, READ_SIZE);
        // XOR the bytes from Quantis with bytes from getrandom
        if (XOR_RANDOM) {
            ssize_t n = getrandom(tmp_buffer, READ_SIZE, 0);

            if (n != READ_SIZE) {
                fprintf(stderr, "Error: read only %zd bytes out of %d\n", n, READ_SIZE);
                return 1;
            }

            for (int i = 0; i < READ_SIZE; i++) {
                buffer[i] ^= tmp_buffer[i];
            }
            memset(tmp_buffer, 0, sizeof(tmp_buffer));
        }
        clock_gettime(CLOCK_MONOTONIC, &readEnd);

        if (bytesRead < 0) {
            fprintf(stderr, "An error occurred when reading random bytes: %s\n", QuantisStrError(bytesRead));
            return 1;
        }

        totalBytesRead += bytesRead;
        double elapsed_time = (readEnd.tv_sec - begin_time.tv_sec) + 1e-9 * (readEnd.tv_nsec - begin_time.tv_nsec);

        if (elapsed_time >= (step_count + 1) * step_duration) {
            mbPerSec[step_count] = (totalBytesRead / elapsed_time) / UNITS;
            times[step_count] = elapsed_time;
            step_count++;
        }

        if (elapsed_time >= total_duration) {
            break;
        }
    }

    return 0;
}

int main(int argc, char *argv[]) {
    char *source = NULL;
    char *mode = NULL;
    int step_duration = -1;
    int total_duration = -1;
    int read_size = -1;

    bool XOR_RANDOM = false;

    if (parse_arguments(argc, argv, &source, &step_duration, &total_duration, &mode, &read_size, &XOR_RANDOM) != 0) {
        return 1;
    }

    int steps = total_duration / step_duration;
    double mbPerSec[total_duration / step_duration];
    double times[steps];


    if (strcmp(mode, "getrandom") == 0) {
        if (get_from_getrandom(mbPerSec, times, step_duration, total_duration) != 0) {
            return 1;
        }
    } else if (strcmp(mode, "quantis") == 0) {
        QuantisDeviceHandle *handle = NULL;
        if (open_quantis(&handle) != 0) {
            return 1;
        }
        if (get_from_quantis(handle, mbPerSec, times, step_duration, total_duration, XOR_RANDOM) != 0) {
            close_quantis(handle);
            return 1;
        }
        close_quantis(handle);
    } else {
        if (get_from_character_device(source, mbPerSec, times, step_duration, total_duration, XOR_RANDOM) != 0) {
            return 1;
        }
    }

    if (create_csv_file(&step_duration, steps, mbPerSec, times) != 0) {
        return 1;
    }

    return 0;
}