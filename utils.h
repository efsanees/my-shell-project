#ifndef UTILS_H
#define UTILS_H

void parse_command(char *input, char *args[]);
int is_background_process(char *input);
int redirect_input(char *filename);
int redirect_output(char *filename);
void handle_pipe(char *input);

#endif
