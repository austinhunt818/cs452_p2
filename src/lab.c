
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <termios.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include "lab.h"

void print_version() {
    printf("Version 1.0.0\n");
}

char *get_prompt(const char *env) {
    char *prompt = getenv(env);
    if (prompt == NULL) {
        prompt = "shell>";
    }
    
    char *result = malloc(strlen(prompt) + 1);
    if (result != NULL) {
        strcpy(result, prompt);
    }
    
    return result;
}

int change_dir(char **dir) {
    UNUSED(dir);
    return -1;
}

char **cmd_parse(char const *line) {
    char** cmd = (char**)malloc(sizeof(char*) * sysconf(_SC_ARG_MAX));

    int i = 0;
    int cmd_index = 0;
    while(line[i] != '\0'){
        char* arg = (char*)malloc(sizeof(char) * 100);
        while(isspace(line[i])){
            i++;
        }

        int j = i;
        while(!isspace(line[j]) && line[j] != '\0'){
            arg[j-i] = line[j];
            j++;
        }
        arg[j-i] = '\0';
        cmd[cmd_index] = arg;
        cmd_index++;
        i = j;
    }

    cmd[cmd_index] = NULL;
    return cmd;
}

void cmd_free(char **line) {
    int i = 0;
    while(line[i] != NULL){
        free(line[i]);
        i++;
    }
    free(line);
}

char *trim_white(char *line) {
    if(line == NULL) {
        return NULL;
    }

    char *trimmed = malloc(strlen(line) + 1);

    int i = 0;
    int j = 0;
    
    //remove from beginning
    while(isspace(line[i])) {
        i++;
    }
    while(line[i] != '\0') {
        trimmed[j] = line[i];
        i++;
        j++;
    }
    
    while(j > 0 && isspace(trimmed[j-1])) {
        j--;
    }
 
    trimmed[j] = '\0';
    return trimmed;
}

bool do_builtin(struct shell *sh, char **argv) {   
    char* EXIT = "exit";
    if (argv == NULL || argv[0] == NULL) {
        return false;
    }
    if(strcmp(argv[0], EXIT) == 0) {
        sh_destroy(sh);
        exit(0);
    }
    else{
        return false;
    }
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

    //give control of the terminal back to the shell
    tcsetpgrp(sh->shell_terminal, sh->shell_pgid);
    tcsetattr(sh->shell_terminal, TCSADRAIN, &sh->shell_tmodes);

    //reset signals
    signal(SIGINT, SIG_DFL);
    signal(SIGQUIT, SIG_DFL);
    signal(SIGTSTP, SIG_DFL);
    signal(SIGTTIN, SIG_DFL);
    signal(SIGTTOU, SIG_DFL);

    //kill the shell
    kill(getpid(), SIGTERM);
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