#ifndef SHELL_H
#define SHELL_H

void print_prompt();
void handle_redirection(char *args[], int *input_fd, int *output_fd);
void execute_command(char *command);
void execute_in_background(char *command);
void execute_piped_commands(char *commands[], int num_commands);
void wait_for_background_processes();

#endif