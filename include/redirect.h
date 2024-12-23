// redirect.h
#ifndef REDIRECT_H
#define REDIRECT_H

void input_redirect(char *file);
void output_redirect(char *file);
void execute_command_with_redirect(char *cmd, char *input_file, char *output_file); // Çıkış yönlendirmesi için fonksiyon prototipi

#endif
