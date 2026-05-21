#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <ctype.h>

int MAX_INPUT = 124;
int MAX_ARGS = 8;

void print_cwd(char *cwd) {
    getcwd(cwd, 1024);
    printf("[%s]# ", cwd);
    fflush(stdout);
}

char* strip(char *line) {

    char *start = line;

    while (isspace(*line) || *line == '|') {
        *line++ = '\0';
    }
    start = line;
    int len = strlen(start);

    while (len > 0 && (isspace(start[len - 1]) || start[len - 1] == '|')) {
        start[len-- - 1] = '\0';
    }

    return start;
}

int format_piped(char *line, char **cmds) {
    int len = strlen(line);
    int i = 0;
    char *delim = "|";
    while ((cmds[i] = strsep(&line, delim)) != NULL) i++;

    return i;
}

int tokenize(char *line, char **args) {
    char *delim = " \t\n";
    int i = 0;

    while ((args[i] = strsep(&line, delim)) != NULL) {
        if (args[i][0] != '\0') i++;
    }

    args[i] = NULL;

    return i;
}

void process_cmds(char **cmds, int tot_cmds) {
    if (tot_cmds == 0) return;

    char *args[MAX_ARGS];

    int tokens = 0;

    int pids[tot_cmds];
    int pipes[tot_cmds - 1][2];

    for (int i = 0; i < tot_cmds - 1; i++) pipe(pipes[i]);

    for (int i = 0; i < tot_cmds; i++) {
        int pid = fork();
        pids[i] = pid;

        if (pid == 0) {
            if (i < tot_cmds - 1) dup2(pipes[i][1], STDOUT_FILENO);
            if (i > 0) dup2(pipes[i - 1][0], STDIN_FILENO);
            
            for (int j = 0; j < tot_cmds - 1; j++) {
                close(pipes[j][0]);
                close(pipes[j][1]);
            }

            tokenize(cmds[i], args);
            execvp(args[0], args);
            perror(args[0]); exit(1);
        } 

    }

    for (int i = 0; i < tot_cmds - 1; i++) {
        close(pipes[i][0]);
        close(pipes[i][1]);
    }

    int status = 0;
    for (int i = 0; i < tot_cmds; i++) {
        waitpid(pids[i], &status, 0);
    }

}

char* get_cmds(char *line, char **cmds) {
    if (fgets(line, MAX_INPUT, stdin) == NULL) { printf("\n"); exit(0); }

    char* start = strip(line);

    int tot_cmds = format_piped(start, cmds);
    process_cmds(cmds, tot_cmds);

    return start;
}


int main(void) {
    char line[MAX_INPUT];
    char *cmds[MAX_INPUT];
    char *args[MAX_ARGS]; // pointers to line
    char *start = NULL;
    char cwd[1024];

    memset(line, '\0', MAX_INPUT);

    while (1) {
        print_cwd(cwd);
        get_cmds(line, cmds);
    }
}
