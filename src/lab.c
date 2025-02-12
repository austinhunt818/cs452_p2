
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "lab.h"

void print_version() {
    printf("Version 1.0.0\n");
}

// int main(int argc, char *argv[]) {
//     int opt;
//     while ((opt = getopt(argc, argv, "v")) != -1) {
//         switch (opt) {
//             case 'v':
//                 print_version();
//                 return 0;
//             default:
//                 fprintf(stderr, "Usage: %s [-v]\n", argv[0]);
//                 return 1;
//         }
//     }
//     return 0;
// }

char *get_prompt(const char *env) {
    return NULL;
}

int change_dir(char **dir) {
    return -1;
}

char **cmd_parse(char const *line) {
    return NULL;
}

void cmd_free(char **line) {
}

char *trim_white(char *line) {
    return NULL;
}

bool do_builtin(struct shell *sh, char **argv) {

    return 0==1;
}

void sh_init(struct shell *sh) {
}

void sh_destroy(struct shell *sh) {
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