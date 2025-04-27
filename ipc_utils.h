#ifndef IPC_UTILS_H
#define IPC_UTILS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <dirent.h>
#include <ctype.h>

#define MAX_WORKERS 10
#define BUFFER_SIZE 1024
#define MAX_RESULTS 100
#define MAX_INDEX_ENTRIES 1000
#define FIFO_NAME_BASE "/tmp/search_fifo_"

typedef struct {
    char filename[256];
    int line_number;
    char line_content[512];
} SearchResult;

typedef struct {
    char word[50];
    SearchResult results[MAX_RESULTS];
    int count;
} WordEntry;

#endif
