#include <stdio.h>
#include <stdlib.h>
#include <unistd.h> 
#include <string.h>
#include <sys/types.h>
#include <signal.h>
#include <sys/wait.h>

//Function to count args.
int getArgsCount(char command[]) {
    int space_counter = 0; //Count spaces in line
    int i;
    space_counter = 0;
    for(i = 0; command[i] != '\0'; i++) {
        if(command[i] == ' ') {
            space_counter = space_counter + 1;
        }
    }
    return space_counter + 2;
}

int getParallelCommandsCount(char line[]) {
    int colon_counter = 0;
    int i;
    colon_counter = 0;
    for(i = 0; line[i] != '\0'; i++) {
        if(line[i] == ';') {
            colon_counter = colon_counter + 1;
        }
    }
    return colon_counter + 1;
}

int getPipedCommandsCount(char line[]) {
    int pipe_counter = 0;
    int i;
    pipe_counter = 0;
    for(i = 0; line[i] != '\0'; i++) {
        if(line[i] == '|') {
            pipe_counter = pipe_counter + 1;
        }
    }
    return pipe_counter + 1;
}

//Tokenize the command and split
int tokenize(char *args[], char *command, char *delimeter) {
    char *token = strtok(command, delimeter);
    int j = 0;
    while(token != NULL) {
        args[j++] = token;
        token = strtok(NULL, delimeter);
    }
    args[j] = NULL;
}

//To run cd command
int run_cd(char *dir) {
    printf("cd %s\n\n", dir);
    if(chdir(dir) < 0) {
        printf("Could not change directory.");
        return -1;
    }
    return 0;
}

//To run history command
int run_history(char *option) {
    if(option != NULL) {

    } else {

    }
}

//To print command
void printCommand(int argc, char *args[]) {
    for(int k = 0; k < argc - 1; k++) {
        printf("%s ", args[k]);
    }
    printf("\n");
}

//To run a command "normally"
int runCommand(char *command) {
    int args_count;
    pid_t pid;

    args_count = getArgsCount(command);
    char *args[args_count];
    tokenize(args, command, " ");

    pid = fork();
    if(pid == 0) {
        execvp(args[0], args);
    }

    return pid;
}



/*
*Main functions
*/

//In-order function.
int runInOrder(FILE *file_pointer) {
    char command[50]; //char array to store commands
    int args_count; //number of args
    pid_t pid;
    char *command_cd = "cd";
    char *command_history = "history";
    int execerr = 0;

    while(fgets(command, 50, file_pointer) != NULL) {
        args_count = getArgsCount(command);
        char *args[args_count];
        tokenize(args, command, " \n");
        
        if(strcmp(args[0], command_cd) == 0) {
            run_cd(args[1]);
        } else if(strcmp(args[0], command_history) == 0) {
            run_history(args[1]);
        } else {
            printCommand(args_count, args);
            
            pid = fork();
            if(pid == 0){
                execerr = execvp(args[0], args);
            }

            if(execerr == 0) {
                wait(NULL);
                kill(pid, SIGTERM);
                printf("\n");
            } else {
                printf("Hata: \"%s\" komutu basarisiz.\n\n", args[0]);
                kill(pid, SIGTERM);
            }
        }
    }
    return 0;
}

//Parallel function
int runParallel(FILE *file_pointer) {
    char line[500];
    int commands_count;
    pid_t pid;
    
    while(fgets(line, 500, file_pointer) != NULL) {
        commands_count = getParallelCommandsCount(line);
        char *commands[commands_count];
        tokenize(commands, line, ";\n");
        
        int i;
        int pids[commands_count];
        for(i = 0; i < commands_count; i++) {
            pids[i] = runCommand(commands[i]);
            printf("\n");
        }

        int j = 0;
        for(j = 0; j < commands_count; j++) {
            waitpid(pids[j], NULL, 0);
            kill(pids[j], SIGTERM);
        }
    }
    return 0;
}

//Pipe function
int runPipe(FILE *file_pointer) {
    int p[2];
    int fd_in = 0;
    char line[500];
    int commands_count;
    int args_count;
    pid_t pid;

    while(fgets(line, 500, file_pointer) != NULL) {
        commands_count = getPipedCommandsCount(line);
        char *commands[commands_count];
        tokenize(commands, line, "|\n");
        int i;
        for(i = 0; i < commands_count; i++) {
            char *command = commands[i];
            args_count = getArgsCount(command);
            char *args[args_count];
            tokenize(args, command, " ");
            
            pipe(p);

            pid = fork();

            if(pid == 0) {
                dup2(fd_in, 0);
                if(i < commands_count - 1) {
                    dup2(p[1], 1);
                }
                close(p[0]);
                execvp(args[0], args);
                exit(EXIT_FAILURE);
            }
            else if(pid == -1) {
                printf("Fork failure.");
            }
            else {
                wait(NULL);
                close(p[1]);
                fd_in = p[0];
            }

        }
        printf("\n");
    }

}

int main(int argc, char *argv[]) {
    FILE *fpointer;
    fpointer = fopen(argv[1], "r");
    if(strcmp(argv[2], "1") == 0) {
        runInOrder(fpointer);
    } else if(strcmp(argv[2], "2") == 0) {
        runParallel(fpointer);
    } else if(strcmp(argv[2], "3") == 0) {
        runPipe(fpointer);
    }
}