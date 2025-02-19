
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <termios.h>
#include <unistd.h>
#include <signal.h>
#include "lab.h"

void print_version() {
    printf("Version 1.0.0\n");
}

char *get_prompt(const char *env) {
    char *prompt = getenv(env);
    if (prompt == NULL) {
        return "shell>";
    }
    
    return prompt;
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
    UNUSED(line);
    return NULL;
}

bool do_builtin(struct shell *sh, char **argv) {   
    UNUSED(sh);
    UNUSED(argv);
    return 0==1;
}

void sh_init(struct shell *sh) {

    //See if running interactively
    sh->shell_terminal = STDIN_FILENO;
    sh->shell_is_interactive = isatty(sh->shell_terminal);

    //loop until on foreground
    if(sh->shell_is_interactive){
        while(tcgetpgrp(sh->shell_terminal) != (sh->shell_pgid = getpgrp()))
            kill(- sh->shell_pgid, SIGTTIN);
    }

    //ignore signals
    signal(SIGINT, SIG_IGN);
    signal(SIGQUIT, SIG_IGN);
    signal(SIGTSTP, SIG_IGN);
    signal(SIGTTIN, SIG_IGN);
    signal(SIGTTOU, SIG_IGN);

    //put ourselves in our own process group
    sh->shell_pgid = getpid();
    if(setpgid(sh->shell_pgid, sh->shell_pgid) < 0){
        perror("Couldn't put the shell in its own process group");
        exit(1);
    }

    //get control of the terminal
    tcsetpgrp(sh->shell_terminal, sh->shell_pgid);
    tcgetattr(sh->shell_terminal, &sh->shell_tmodes);

    //set prompt
    sh->prompt = get_prompt("MY_PROMPT");

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