
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "lab.h"

void print_version() {
    printf("Version 1.0.0\n");
}

char *get_prompt(const char *env) {
    UNUSED(env);
    return NULL;
}

int change_dir(char **dir) {
    UNUSED(dir);
    return -1;
}

char **cmd_parse(char const *line) {
    UNUSED(line);
    return NULL;
}

void cmd_free(char **line) {
    UNUSED(line);
}

char *trim_white(char *line) {
    char* trimmed;
    int i = 0;
    while(line[i] != '\0') {
        if (!isspace(line[i])) {
            trimmed += line[i];
        }
    }
    return trimmed;
}

bool do_builtin(struct shell *sh, char **argv) {   
    UNUSED(sh);
    UNUSED(argv);
    return 0==1;
}

void sh_init(struct shell *sh) {
    UNUSED(sh);
}

void sh_destroy(struct shell *sh) {
    UNUSED(sh);
}

void parse_args(int argc, char **argv) {
    int opt;
    while ((opt = getopt(argc, argv, "v")) != -1) {
        switch (opt) {
            case 'v':
                printf("Version %d.%d\n", lab_VERSION_MAJOR, lab_VERSION_MINOR);
                exit(0);
            default:
                fprintf(stderr, "Usage: %s [-v]\n", argv[0]);
                exit(1);
        }
    }
}