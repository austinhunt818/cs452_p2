
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <termios.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <pwd.h>
#include "lab.h"
#include <readline/history.h>


char *get_prompt(const char *env) {
    //get prompt from environment variable
    char *prompt = getenv(env);

    //default prompt
    if (prompt == NULL) {
        prompt = "shell>";
    }  
    
    //Copy prompt to new memory
    char *result = malloc(strlen(prompt) + 1);
    if (result != NULL) {
        strcpy(result, prompt);
    }
    
    return result;
}

int change_dir(char **dir) {
    int result;

    //if no arguments, change to home directory
    if(dir[1] == NULL){
        if(getenv("HOME") == NULL){
            //get home directory from password file if no environment variable
            result = chdir(getpwuid(getuid())->pw_dir);
        }
        result = chdir(getenv("HOME"));
        return result;
    }
    result = chdir(dir[1]);

    //print error if invalid directory
    if(result == -1){
        perror("chdir");
    }
    return result;
}

char **cmd_parse(char const *line) {
    if(line == NULL) {
        return NULL;
    }

    //allocate memory for command to size of max arguments from sysconf
    char** cmd = (char**)malloc(sizeof(char*) * sysconf(_SC_ARG_MAX));

    //parse line into arguments
    int i = 0;
    int cmd_index = 0;
    while(line[i] != '\0'){
        char* arg = (char*)malloc(sizeof(char) * 100);

        //skip whitespace
        while(isspace(line[i])){
            i++;
        }

        //copy argument into arg variable
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

    //null terminate the array
    cmd[cmd_index] = NULL;
    return cmd;
}

void cmd_free(char **line) {
    int i = 0;

    //free the cmd and each argument
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

    //copy the line after initial whitespace to trimmed
    while(line[i] != '\0') {
        trimmed[j] = line[i];
        i++;
        j++;
    }
    
    //remove from end
    while(j > 0 && isspace(trimmed[j-1])) {
        j--;
    }
    trimmed[j] = '\0';

    return trimmed;
}

bool do_builtin(struct shell *sh, char **argv) {   
    if (argv == NULL || argv[0] == NULL) {
        return false;
    }

    //exit command
    if(strcmp(argv[0], "exit") == 0) {
        sh_destroy(sh);
        exit(0);
    }
    //cd command
    else if(strcmp(argv[0], "cd") == 0) {
        change_dir(argv);
        return true;
    }
    //history command
    else if(strcmp(argv[0], "history") == 0) {
        HIST_ENTRY **history = history_list();
        if(history == NULL) {
            return false;
        }
        //print history
        for(int i = 0; history[i] != NULL; i++) {
            printf("%d %s\n", i, history[i]->line);
        }
        return true;
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

    //free the prompt
    free(sh->prompt);

    //kill the shell
    kill(getpid(), SIGTERM);
}

void parse_args(int argc, char **argv) {
    int opt;
    //parse command line arguments
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