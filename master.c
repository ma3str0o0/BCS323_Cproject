#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#define MAX_WORKERS 10
#define MAX_PATH 1024

typedef struct {
    int num_workers;
    char directories[MAX_WORKERS][MAX_PATH];
    char file_types[MAX_WORKERS][20];
} Config;

// Reads the configuration file and stores values in the config struct
void read_config(Config *config) {
    FILE *file = fopen("config.txt", "r");
    if (!file) {
        perror("Failed to open config.txt");
        exit(1);
    }

    char line[256];
    while (fgets(line, sizeof(line), file)) {
        if (strncmp(line, "NUM_WORKERS=", 12) == 0) {
            config->num_workers = atoi(line + 12);
        } else if (strncmp(line, "DIRECTORIES=", 12) == 0) {
            char *token = strtok(line + 12, ",\n");
            int i = 0;
            while (token && i < MAX_WORKERS) {
                strncpy(config->directories[i++], token, MAX_PATH);
                token = strtok(NULL, ",\n");
            }
        } else if (strncmp(line, "FILE_TYPES=", 11) == 0) {
            char *token = strtok(line + 11, ",\n");
            int i = 0;
            while (token && i < MAX_WORKERS) {
                strncpy(config->file_types[i++], token, 20);
                token = strtok(NULL, ",\n");
            }
        }
    }

    fclose(file);
}

// Main function: forks workers and waits for them to finish
int main() {
    Config config;
    read_config(&config);

    pid_t pids[MAX_WORKERS];

    for (int i = 0; i < config.num_workers; i++) {
        pid_t pid = fork();
        if (pid == 0) {
            // Child process: start a worker with directory and file type as args
            char *args[] = {
                "./worker",
                config.directories[i],
                config.file_types[i],
                NULL
            };
            execvp(args[0], args);
            perror("Failed to launch worker");
            exit(1);
        } else if (pid > 0) {
            // Parent: store child PID
            pids[i] = pid;
        } else {
            perror("fork() failed");
            exit(1);
        }
    }

    // Wait for all workers to finish
    for (int i = 0; i < config.num_workers; i++) {
        waitpid(pids[i], NULL, 0);
    }

    return 0;
}