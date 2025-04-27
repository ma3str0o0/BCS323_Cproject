#include "ipc_utils.h"
#include <sys/wait.h>

WordEntry word_index[MAX_INDEX_ENTRIES];
int entry_count = 0;
int indexing_complete = 0;

int strcasecmp(const char *s1, const char *s2) {
    while (*s1 && *s2) {
        int diff = tolower(*s1) - tolower(*s2);
        if (diff != 0) return diff;
        s1++;
        s2++;
    }
    return tolower(*s1) - tolower(*s2);
}

WordEntry* get_word_entry(const char *word) {
    for (int i = 0; i < entry_count; i++) {
        if (strcasecmp(word_index[i].word, word) == 0) {
            return &word_index[i];
        }
    }
    
    if (entry_count < MAX_INDEX_ENTRIES) {
        strncpy(word_index[entry_count].word, word, sizeof(word_index[0].word));
        word_index[entry_count].count = 0;
        return &word_index[entry_count++];
    }
    return NULL;
}

void index_file(const char *filename) {
    printf("Indexing file: %s\n", filename);
    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("Failed to open file");
        return;
    }

    char line[512];
    int line_num = 0;
    
    while (fgets(line, sizeof(line), file)) {
        line_num++;
        char line_copy[512];
        strcpy(line_copy, line);
        
        char *word = strtok(line_copy, " \t\n\r");
        
        while (word != NULL) {
            if (strlen(word) >= 3) {
                WordEntry *entry = get_word_entry(word);
                if (entry && entry->count < MAX_RESULTS) {
                    SearchResult *res = &entry->results[entry->count++];
                    strncpy(res->filename, filename, sizeof(res->filename));
                    res->line_number = line_num;
                    strncpy(res->line_content, line, sizeof(res->line_content));
                    printf("  Indexed word: '%s' in %s:%d\n", word, filename, line_num);
                }
            }
            word = strtok(NULL, " \t\n\r");
        }
    }
    fclose(file);
}

void index_files(const char *directory, const char *file_types) {
    DIR *dir;
    struct dirent *entry;
    
    if (!(dir = opendir(directory))) {
        perror("opendir failed");
        return;
    }

    char types_copy[50];
    strcpy(types_copy, file_types);
    char *extensions[10];
    int ext_count = 0;
    char *token = strtok(types_copy, ",");
    
    while (token != NULL && ext_count < 10) {
        extensions[ext_count++] = token;
        token = strtok(NULL, ",");
    }

    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_type == DT_REG) {
            char *ext = strrchr(entry->d_name, '.');
            if (ext) {
                int match = 0;
                for (int i = 0; i < ext_count; i++) {
                    if (strcmp(ext, extensions[i]) == 0) {
                        match = 1;
                        break;
                    }
                }
                if (!match) continue;

                char filepath[512];
                snprintf(filepath, sizeof(filepath), "%s/%s", directory, entry->d_name);
                index_file(filepath);
            }
        }
    }
    closedir(dir);
    indexing_complete = 1;
    printf("Worker finished indexing (%d words indexed)\n", entry_count);
}

void search_index(const char *query, int worker_id) {
    printf("Worker %d searching for '%s'\n", worker_id, query);
    printf("Current index contains %d words\n", entry_count);
    
    WordEntry *entry = get_word_entry(query);
    
    if (entry && entry->count > 0) {
        printf("Worker %d found %d matches for '%s':\n", 
               worker_id, entry->count, query);
        
        for (int i = 0; i < entry->count; i++) {
            SearchResult *res = &entry->results[i];
            printf("  %s:%d: %s", res->filename, res->line_number, res->line_content);
        }
    } else {
        printf("Worker %d: No matches found for '%s'\n", worker_id, query);
    }
}

void handle_queries(int worker_id) {
    char fifo_name[50];
    sprintf(fifo_name, "%s%d", FIFO_NAME_BASE, worker_id);
    
    int fd = open(fifo_name, O_RDONLY);
    if (fd < 0) {
        perror("open fifo failed");
        return;
    }

    char query[BUFFER_SIZE];
    while (read(fd, query, BUFFER_SIZE) > 0) {
        if (!indexing_complete) {
            printf("Worker %d: Indexing not complete yet\n", worker_id);
            continue;
        }
        search_index(query, worker_id);
    }
    close(fd);
}

int main(int argc, char *argv[]) {
    if (argc < 4) {
        fprintf(stderr, "Usage: %s <worker_id> <directory> <file_types>\n", argv[0]);
        exit(1);
    }
    int worker_id = atoi(argv[1]);
    printf("Worker %d started\n", worker_id);
    index_files(argv[2], argv[3]);
    handle_queries(worker_id);

    return 0;
}
