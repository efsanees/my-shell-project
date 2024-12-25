#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <signal.h>

#define MAX_COMMAND_LENGTH 1024
#define MAX_ARGS 128
#define MAX_BACKGROUND_PROCESSES 128

// Background process tracking
pid_t background_pids[MAX_BACKGROUND_PROCESSES];
int background_pid_count = 0;

// Function to print the shell prompt
void print_prompt() {
    printf("> ");
    fflush(stdout); // Ensure prompt is immediately visible
}

// Function to handle input and output redirection
void handle_redirection(char *args[], int *input_fd, int *output_fd) {
    for (int i = 0; args[i] != NULL; i++) {
        if (strcmp(args[i], "<") == 0) {
            *input_fd = open(args[i + 1], O_RDONLY);
            if (*input_fd < 0) {
                fprintf(stderr, "%s giriş dosyası bulunamadı.\n", args[i + 1]);
                return;
            }
            args[i] = NULL;
        } else if (strcmp(args[i], ">") == 0) {
            *output_fd = open(args[i + 1], O_WRONLY | O_CREAT | O_TRUNC, 0644);
            if (*output_fd < 0) {
                perror("Output file error");
                exit(EXIT_FAILURE);
            }
            args[i] = NULL;
        }
    }
}

// Function to execute a single command
void execute_command(char *command) {
    char *args[MAX_ARGS];
    int input_fd = -1, output_fd = -1;
    int i = 0;

    // Tokenize the command
    char *token = strtok(command, " ");
    while (token != NULL) {
        args[i++] = token;
        token = strtok(NULL, " ");
    }
    args[i] = NULL;

    // Handle I/O redirection
    handle_redirection(args, &input_fd, &output_fd);

    pid_t pid = fork();
    if (pid == 0) {
        // Child process
        if (input_fd != -1) {
            dup2(input_fd, STDIN_FILENO);
            close(input_fd);
        }
        if (output_fd != -1) {
            dup2(output_fd, STDOUT_FILENO);
            close(output_fd);
        }
        execvp(args[0], args);
        perror("Command execution failed");
        exit(EXIT_FAILURE);
    } else if (pid > 0) {
        int status;
        waitpid(pid, &status, 0);  // Parent process waits for the child
    } else {
        perror("Fork failed");
    }
}

// Function to handle background processes
void execute_in_background(char *command) {
    char *args[MAX_ARGS];
    int i = 0;

    // Tokenize the command
    char *token = strtok(command, " ");
    while (token != NULL) {
        args[i++] = token;
        token = strtok(NULL, " ");
    }
    args[i] = NULL;

    // Check if the command is 'sleep' and print a message
    if (strcmp(args[0], "sleep") == 0) {
        printf("The 'sleep' command is running in the background.\n");
    }

    pid_t pid = fork();
    if (pid == 0) {
        // Child process
        execvp(args[0], args);
        perror("Background command execution failed");
        exit(EXIT_FAILURE);
    } else if (pid > 0) {
        // Parent process does not wait for the child
        background_pids[background_pid_count++] = pid; // Track background PID
        printf("[Background PID %d]\n", pid);
    } else {
        perror("Fork failed");
    }
}

// Function to execute commands connected by pipes
void execute_piped_commands(char *commands[], int num_commands) {
    int pipe_fd[2];
    int input_fd = STDIN_FILENO;

    for (int i = 0; i < num_commands; i++) {
        // Create pipe
        if (pipe(pipe_fd) == -1) {
            perror("Pipe creation failed");
            exit(EXIT_FAILURE);
        }

        pid_t pid = fork();
        if (pid == 0) {
            // Child process

            // Redirect input from previous pipe (if not the first command)
            dup2(input_fd, STDIN_FILENO);
            close(input_fd);

            // If this is not the last command, redirect output to the next pipe
            if (i < num_commands - 1) {
                dup2(pipe_fd[1], STDOUT_FILENO);
            }
            close(pipe_fd[0]);
            close(pipe_fd[1]);

            // Parse the current command into arguments
            char *args[MAX_ARGS];
            int j = 0;
            char *token = strtok(commands[i], " ");
            while (token != NULL) {
                args[j++] = token;
                token = strtok(NULL, " ");
            }
            args[j] = NULL;

            // Execute the command
            execvp(args[0], args);
            perror("Command execution failed");
            exit(EXIT_FAILURE);
        } else if (pid > 0) {
            // Parent process
            close(pipe_fd[1]);

            // The input for the next command is the read end of the current pipe
            input_fd = pipe_fd[0];
        } else {
            perror("Fork failed");
            exit(EXIT_FAILURE);
        }
    }

    // Wait for all child processes to finish
    for (int i = 0; i < num_commands; i++) {
        wait(NULL);
    }
}

// Function to wait for all background processes to finish and report exit status
void wait_for_background_processes() {
    for (int i = 0; i < background_pid_count; i++) {
        int status;
        pid_t pid = waitpid(background_pids[i], &status, WNOHANG);  // Non-blocking wait
        if (pid > 0) {
            printf("[%d] retval: %d\n", pid, WEXITSTATUS(status)); // Print PID and exit status
        }
    }
}

// Main shell loop
int main() {
    char command[MAX_COMMAND_LENGTH];

    while (1) {
        print_prompt();

        // Read user input
        if (fgets(command, sizeof(command), stdin) == NULL) {
            break; // Exit on EOF
        }

        // Remove trailing newline character from command
        command[strcspn(command, "\n")] = '\0';

        // Handle built-in "quit" command
        if (strcmp(command, "quit") == 0) {
            printf("Exiting shell...\n");

            // Wait for background processes to finish before exiting
            wait_for_background_processes();
            break;
        }

        // Check if there's a background process (command ends with '&')
        if (strchr(command, '&') != NULL) {
            *strchr(command, '&') = '\0'; // Remove '&'
            execute_in_background(command);
            continue;  // Continue with the next input immediately
        }

        // Check for pipes
        else if (strchr(command, '|') != NULL) {
            char *commands[MAX_ARGS];
            int num_commands = 0;

            char *token = strtok(command, "|");
            while (token != NULL) {
                commands[num_commands++] = token;
                token = strtok(NULL, "|");
            }
            execute_piped_commands(commands, num_commands);
        }
        // Check for semicolon separated commands
        else if (strchr(command, ';') != NULL) {
            char *commands[MAX_ARGS];
            int num_commands = 0;

            char *token = strtok(command, ";");
            while (token != NULL) {
                commands[num_commands++] = token;
                token = strtok(NULL, ";");
            }
            // Execute each command sequentially
            for (int i = 0; i < num_commands; i++) {
                execute_command(commands[i]);
            }
        }
        // Execute single command
        else {
            execute_command(command);
        }

        // After every input, check for finished background processes
        wait_for_background_processes();
    }

    return 0;
}

