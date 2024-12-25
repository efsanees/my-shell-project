#ifndef BACKGROUND_H
#define BACKGROUND_H

void handle_background(char *command);
void wait_for_all_background_processes();
void background_process_handler(int sig);

#endif
