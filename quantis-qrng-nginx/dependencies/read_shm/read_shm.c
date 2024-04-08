#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>

#define SHARED_MEM_NAME "/random_numbers_shm"

int main(int argc, char *argv[]) {
    int shm_fd;
    size_t *counter;
    FILE *fp;

    shm_fd = shm_open(SHARED_MEM_NAME, O_RDONLY, 0);
    if (shm_fd == -1) {
        perror("shm_open");
        return EXIT_FAILURE;
    }

    counter = mmap(NULL, sizeof(size_t), PROT_READ, MAP_SHARED, shm_fd, 0);
    if (counter == MAP_FAILED) {
        perror("mmap");
        close(shm_fd);
        return EXIT_FAILURE;
    }


    printf("Counter value: %zu\n", *counter);

    fp = fopen("output.csv", "a");
    if (fp == NULL) {
        perror("fopen");
        munmap(counter, sizeof(size_t)); // Clean up memory mapping
        close(shm_fd); // Close shared memory object
        return EXIT_FAILURE;
    }

    fprintf(fp, "%zu\n", *counter);

    if (munmap(counter, sizeof(size_t)) == -1) {
        perror("munmap");
    }

    close(shm_fd);

    fclose(fp);

    return EXIT_SUCCESS;
}