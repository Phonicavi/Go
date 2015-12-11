/*用来debug的类*/

#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>


void debug_log_int(int i);
void debug_log(char *s);
void debug_log_board_status(board_status *bs);
void print_string(char* s);
void print_line(char* s);
int log_txt(char *content);
void debug_c(char c);