
#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include "board.h"
#include <string.h>


void print_log(char* s){
	printf("%s\n", s);
}

void print_line(char* s){
	//printf("===============%s====================\n",s);
}


int log_txt(char *content){
	FILE *fp;//����һ���ļ�����ָ��
	fp = fopen("log.txt", "w+");//��׷�ӵķ�ʽ�������1.txt��Ĭ��λ����������Ŀ¼����
	fprintf(fp, "%s\n", content);//ͬ���printfһ�����Ը�ʽ��ʽ������ı���
	fclose(fp);//�ر���

	return 1;
}


void debug_log_int(int i)
{
	FILE *debug_file;
	debug_file = fopen("debug.log", "a");
	fprintf(debug_file, "%d->", i);
	fclose(debug_file);
}

void debug_log(char *s)
{
	FILE *debug_file;
	debug_file = fopen("debug.log", "a");
	fprintf(debug_file, "%s", s);
	fclose(debug_file);
}

void debug_c(char c)
{
	FILE *debug_file;
	debug_file = fopen("debug.log", "a");
	fprintf(debug_file, "%c", c);
	fclose(debug_file);
}


static char color_to_char(intersection color) {
	if (color == BLACK)
		return 'X';
	if (color == WHITE)
		return 'O';
	return '.';
}

void debug_log_board_status(board_status *bs) {
	int i, j;
	FILE *debug_file;
	debug_file = fopen("debug.log", "a");

	fprintf(debug_file, " #");
	for (j = 0; j < board_size; j++)
		fprintf(debug_file, "%2d", j);
	fprintf(debug_file, "\n");
	for (i = 0; i < board_size; i++) {
		fprintf(debug_file, "%2d", i);
		for (j = 0; j < board_size; j++)
			fprintf(debug_file, "%2c", color_to_char(bs->board[POS(i, j)]));
		fprintf(debug_file, "\n");
	}
	fprintf(debug_file, "\n");

	fclose(debug_file);
}
