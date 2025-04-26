#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>

#define MAX_WORKERS 10
#define MAX_PATH 256

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Usage: %s \"search_term\"\n", argv[0]);
        exit(1);
    }

    FILE *config = fopen("config.txt", "r");
    if (!config) {
        perror("Failed to open config.txt");
        exit(1);
    }

    int num_workers = 0;
    char line[512];

    while (fgets(line, sizeof(line), config)) {
        if (strncmp(line, "NUM_WORKERS=", 12) == 0) {
            num_workers = atoi(line + 12);
            break;
        }
    }

    fclose(config);

    printf("Search results for: \"%s\"\n", argv[1]);

    for (int i = 0; i < num_workers; i++) {
        char req_fifo[MAX_PATH];
        char res_fifo[MAX_PATH];
        snprintf(req_fifo, sizeof(req_fifo), "worker_%d_req", i);
        snprintf(res_fifo, sizeof(res_fifo), "worker_%d_res", i);
        int req_fd = open(req_fifo, O_WRONLY);
        if (req_fd == -1) {
            perror("Error opening request FIFO");
            continue;
        }

        write(req_fd, argv[1], strlen(argv[1]));
        close(req_fd);
        int res_fd = open(res_fifo, O_RDONLY);
        if (res_fd == -1) {
            perror("Error opening response FIFO");
            continue;
        }
        
        char buffer[1024];
        int bytes_read;
        while ((bytes_read = read(res_fd, buffer, sizeof(buffer) - 1)) > 0) {
            buffer[bytes_read] = '\0';
            printf("%s", buffer);
        }
        close(res_fd);
    }
    return 0;
}
