#include <stdio.h>
#include <pthread.h>
#include <time.h>
#include <unistd.h>
#include <fcntl.h>

#define READ_SIZE 1024
#define DURATION 60
#define NUM_THREADS 1000 // Number of threads to use
#ifdef USE_OPENSSL
#include <openssl/rand.h>
#endif

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

void* read_random(void* arg) {
    size_t* totalBytesRead = (size_t*) arg;
    int randomData = open("/dev/random", O_RDONLY);
    if (randomData < 0) {
        perror("Could not access /dev/random");
        return NULL;
    }

    char buffer[READ_SIZE];
    ssize_t bytesRead;
    time_t startTime = time(NULL);

    while (time(NULL) - startTime < DURATION) {
        #ifdef USE_OPENSSL
            if (!RAND_bytes((unsigned char*)buffer, READ_SIZE)) {
                fprintf(stderr, "Error using RAND_bytes for generating random data\n");
                return NULL;
            }
            bytesRead = READ_SIZE;
        #else
            bytesRead = read(randomData, buffer, READ_SIZE);
        #endif
        if (bytesRead < 0) {
            perror("Error reading /dev/random");
            return NULL;
        }
        pthread_mutex_lock(&mutex);
        *totalBytesRead += bytesRead;
        pthread_mutex_unlock(&mutex);
    }

    close(randomData);
    return NULL;
}

int main(void) {
    #ifdef USE_OPENSSL
        printf("Using OpenSSL...\n");
    #endif
    size_t totalBytesRead = 0;
    pthread_t threads[NUM_THREADS];

    for(int i = 0; i < NUM_THREADS; i++) {
        pthread_create(&threads[i], NULL, read_random, &totalBytesRead);
    }

    for(int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }

    double mbPerSec = (double)totalBytesRead / (1024.0 * 1024.0) / DURATION;
    printf("Average rate: %.4f MB/sec\n", mbPerSec);

    return 0;
}