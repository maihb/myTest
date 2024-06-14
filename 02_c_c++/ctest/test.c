#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/wait.h>

int main() {
    int *data = mmap(NULL, 10 * sizeof(int), PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if (data == MAP_FAILED) {
        perror("mmap failed");
        exit(1);
    }

    pid_t pid = fork();

    if (pid < 0) {
        perror("fork failed");
        exit(1);
    } else if (pid == 0) {
        // Child process
        printf("Child process, PID: %d\n", getpid());
        data[0] = 42;  // Modify memory in child
        printf("Child modified data[0]: %d\n", data[0]);
        exit(0);
    } else {
        // Parent process
        wait(NULL);  // Wait for child to finish
        printf("Parent process, PID: %d\n", getpid());
        printf("Parent data[0]: %d\n", data[0]);  // Should remain unchanged
    }

    munmap(data, 10 * sizeof(int));
    return 0;
}
