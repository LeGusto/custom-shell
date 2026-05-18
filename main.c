#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

int MAX_INPUT = 124;
int MAX_ARGS = 8;
char *delim = " \t\n";

int tokenize(char *line, char **argv) {
    char *ptr = line;
    int i = 0;
    while ((argv[i] = strsep(&ptr, delim)) != NULL && i < MAX_ARGS - 1) {
        if (argv[i][0] != '\0') i++;  // skip empty tokens
    }

    return i;
}

int execute(char **argv, int args) {
    if (args < 1) return 0;
    char *cmd = argv[0];

    if (strcmp(cmd, "cd") == 0) {
        if (argv[1] == NULL)
            chdir(getenv("HOME"));
        else
            chdir(argv[1]);
    } else if (strcmp(cmd, "exit") == 0) {
        exit(0);
    } else {
        int pid = fork();
        if (pid == 0) {
            execvp(cmd, argv);
            perror(cmd);
            exit(1);
        } else {
            int status;
            waitpid(pid, &status, 0);
            if (WIFEXITED(status))
                printf("[exited %d]\n", WEXITSTATUS(status));
            else if (WIFSIGNALED(status))
                printf("[killed by signal %d]\n", WTERMSIG(status));
        }
    }

    return 0;
}

int main(void) {
    char line[MAX_INPUT];
    char *args[MAX_ARGS]; // pointers to line
    char cwd[1024];

    while (1) {
        getcwd(cwd, sizeof(cwd));
        printf("[%s]$ ", cwd);
        fflush(stdout);

        fgets(line, MAX_INPUT, stdin);
        // printf("%s", line);

        int tokens = tokenize(line, args);
        printf("%d tokens\n", tokens);
        for (int i = 0; i < tokens; i++) {
            printf("%s, ", args[i]);
        }
        printf("\n");

        execute(args, tokens);
    }
}
