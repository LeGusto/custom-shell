#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

int MAX_INPUT = 124;
int MAX_ARGS = 8;

int tokenize(char *line, char **argv) {
    int i = 0;
    argv[i] = strtok(line, " \t\n");
    while (argv[i] != NULL && i < MAX_ARGS - 1) {
        argv[++i] = strtok(NULL, " \t\n"); 
        // NULL lets it continue where it left off
    }

    return i;
}

int main(void) {
    char line[MAX_INPUT];
    char *args[MAX_ARGS]; // pointers to line

    while (1) {
        printf("$ ");
        fflush(stdout);

        fgets(line, MAX_INPUT, stdin);
        // printf("%s", line);

        int tokens = tokenize(line, args);
        printf("%d tokens\n", tokens);
        for (int i = 0; i < tokens; i++) {
            printf("%s, ", args[i]);
        }
        printf("\n");
    }
}
