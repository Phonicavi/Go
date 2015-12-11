#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "board.h"
#include "aplus.h"
#include "tricks_chess_manual.h"
#include "bit_process.h"
#include "string.h"
#include <windows.h>

/***********************************************************************************
* ģ�飺������������
*
* ��������
* int get_manual_start_pos(board_status *bs, intersection color)
*
* ���ߣ�
* *int get_manual_start_pos_sub(board_status *bs, intersection color, int offset_x, int offset_y, int is_oppsite)
*	*���������������Ӻ����������Ѿ�������ĸ�����7*7������ת��ת����״̬������û��������������ƥ���
*
*
***********************************************************************************/
void get_manual_start_pos(board_status *bs, intersection color, int result[])
{
	int i, j, k, offset_x, offset_y;
	//kogo��Ҫ1,5��ʯ�﷼���Ҫ0,6
	int min_offset = 1;
	int max_offset = 5;
	switch ((bs->path_top + 2) % 2)
	{
	case 1:
		for (offset_x = min_offset; offset_x <= max_offset; ++offset_x)
			for (offset_y = min_offset; offset_y <= max_offset; ++offset_y)
			{
			get_manual_start_pos_sub(bs, color, offset_x, offset_y, 0, result);
			}
		break;
	case 0:
		for (offset_x = max_offset; offset_x >= min_offset; --offset_x)
			for (offset_y = max_offset; offset_y >= min_offset; --offset_y)
			{
			get_manual_start_pos_sub(bs, color, offset_x, offset_y, 1, result);
			}
		break;
	default:
		;
	}
}

void get_manual_start_pos_sub(board_status *bs, intersection color, int offset_x, int offset_y, int is_oppsite, int result[])
{
	//����
	int i, j, k, num_stone = 0;
	config *temp;
	for (i = offset_x; i < offset_x + 7; ++i)
	{
		for (j = offset_y; j < offset_y + 7; ++j)
		{
			if (bs->board[POS(i, j)] != EMPTY)
				num_stone++;
			if (num_stone > bs->path_top)
				break;
		}
		if (num_stone > bs->path_top)
			break;
	}
	//�ȷ�ת����Ƚ�
	int temp_board[8][7][7] = { 0 };//8��״̬��7*7
	for (i = offset_x; i < offset_x + 7; ++i)
	{
		for (j = offset_y; j < offset_y + 7; ++j)
		{
			//0
			temp_board[0][i - offset_x][j - offset_y] = bs->board[POS(i, j)];
			if (is_oppsite == 1)
			{
				temp_board[0][6 - (i - offset_x)][6 - (j - offset_y)] = temp_board[0][i - offset_x][j - offset_y];
			}
			//0 ��ת
			temp_board[1][j - offset_y][i - offset_x] = temp_board[0][i - offset_x][j - offset_y];
			//90
			temp_board[2][i - offset_x][6 - (j - offset_y)] = temp_board[0][i - offset_x][j - offset_y];
			//90��ת
			temp_board[3][6 - (j - offset_y)][i - offset_x] = temp_board[2][i - offset_x][6 - (j - offset_y)];
			//180
			temp_board[4][6 - (i - offset_x)][6 - (j - offset_y)] = temp_board[0][i - offset_x][j - offset_y];
			//180��ת
			temp_board[5][6 - (j - offset_y)][6 - (i - offset_x)] = temp_board[4][6 - (i - offset_x)][6 - (j - offset_y)];
			//270
			temp_board[6][6 - (i - offset_x)][j - offset_y] = temp_board[0][i - offset_x][j - offset_y];
			//270��ת
			temp_board[7][j - offset_y][6 - (i - offset_x)] = temp_board[6][6 - (i - offset_x)][j - offset_y];

		}
	}
	//��ϣ���Ƚ�
	if (num_stone >= MANUAL_HASH_TABLE_SIZE)
		num_stone = MANUAL_HASH_TABLE_SIZE - 1;
	for (k = num_stone; k >= num_stone / 2; --k)//num_stone/2����Ѿ�һ�붼���������׵ģ��ǿ϶��������׵���
	{
		temp = hash_array[k].head;
		while (1)
		{
			if (temp == NULL)
				break;

			if (temp->edge_son == NULL || !bit_compare_2(temp->board_bit, temp_board) || !bit_compare_2_conver(temp->board_bit, temp_board))
			{
				temp = temp->next;
			}
			else
			{
				if (is_legal_move(bs, color, temp->edge_son->pos + POS(offset_x, offset_y))
					&& is_fine_move(temp->edge_son->pos + POS(offset_x, offset_y)))
				{
					if (result[temp->edge_son->pos + POS(offset_x, offset_y)] >= 0)
						result[temp->edge_son->pos + POS(offset_x, offset_y)] += SCORE_MANUAL_HIT;
					temp = temp->next;
				}
				else
				{
					temp = temp->next;
				}
			}
		}
	}
}

//���׵ó��������ò���
int is_fine_move(int pos)
{
	int i = I(pos);
	int j = J(pos);
	if (i == 0 || j == 0 || i == MAX_BOARD - 1 || j == MAX_BOARD - 1 || (i >= 5 && i <= 7 && j >= 5 && j <= 7))
		return 0;
	else
		return 1;
}

/***********************************************************************************
* ģ�飺�������ף�����������
*
* ���㺯����
* *void init_trick_chess_manual()
*
* ���ߣ�
* *void init_hash_array()
*	*��������ʼ����ϣ��
*
* *void make_chess_manual_tree()
*	*�������ļ���һ�����ַ�ȥ����
*
* *void refresh_config(config **con, int i, int j, intersection color)
*	*����һ���ӣ�ȥ�������һ����֣������������
*
* *void init_config(config *con, U64 k, bitboard *bb, intersection c, short nc)
* *void init_config_edge(config_edge *edge, int p, config *con, config_edge *brother)
*	*Ϊ��ָ�ֵ�Ĳ���
*
*
***********************************************************************************/

void init_trick_chess_manual()
{

//	printf("before hash\n");
	init_hash_array();

//	printf("after hash\n");

	clock_t start, finish;
	double  duration;
	start = clock();

	make_chess_manual_tree();

//	printf("after chess_tree\n");
	////��ӡ������������
	//int i;
	//for (i = 0; i < STEPS_MANUAL_MAX_BOARD; ++i)
	//{
	//	debug_log_int(i);
	//	config *temp_config = hash_array[(i + 1) % 2][i].head;
	//	while (temp_config != NULL)
	//	{
	//		debug_log_int(temp_config->times);
	//		temp_config = temp_config->next;
	//	}
	//	debug_c('\n');
	//}

	finish = clock();
	duration = (double)(finish - start) / CLOCKS_PER_SEC;
	//printf("make_chess_manual_tree() time: %f seconds\n", duration);
}

void init_hash_array()
{
	int i;
	for (i = 0; i < MANUAL_HASH_TABLE_SIZE; ++i)
	{
		hash_array[i].head = NULL;
	}

	//��ʼ״̬����ǰ��������0���㣬�ֵ���ɫ����
	hash_array[0].head = (config*)malloc(sizeof(config));
	bitboard *temp_bb = (bitboard*)malloc(MAX_BOARD*sizeof(bitboard));
	memset(temp_bb, 0, MAX_BOARD*sizeof(bitboard));
	init_config(hash_array[0].head, temp_bb, BLACK, 0);

}

void init_config(config *con, bitboard *bb, intersection c, short nc)
{
	int i;
	for (i = 0; i < MAX_BOARD; ++i)
	{
		con->board_bit[i] = bb[i];
	}
	con->color = c;
	con->num_current = nc;
	con->edge_son = NULL;
	con->next = NULL;
	con->times = 0;
}

void init_config_edge(config_edge *edge, int p, config *con, config_edge *brother)
{
	//*edge->play_move = bb;
	edge->pos = p;
	edge->config_son = con;
	edge->brother = brother;
}

void refresh_config(config **con, int i, int j, intersection color)
{
	intersection other_color;
	if (color == WHITE)
		other_color = BLACK;
	else
		other_color = WHITE;

	if (*con == NULL)
	{
		//debug_c('@');
		*con = (config*)malloc(sizeof(config));
		bitboard *temp_bb = (bitboard*)malloc(MAX_BOARD*sizeof(bitboard));
		memset(temp_bb, 0, MAX_BOARD*sizeof(bitboard));
		bit_play_move(temp_bb, i, j, color);
		init_config((*con), temp_bb, other_color, 1);

		config_edge *con_edge_new = (config_edge*)malloc(sizeof(config_edge));
		init_config_edge(con_edge_new, POS(i, j), (*con), NULL);

		if (hash_array[0].head->edge_son == NULL)
		{
			hash_array[0].head->edge_son = con_edge_new;
		}
		else
		{
			config_edge *temp = hash_array[0].head->edge_son->brother;
			while (temp != NULL)
			{
				temp = temp->brother;
			}
			temp = con_edge_new;
		}

		(*con)->next = hash_array[1].head;
		hash_array[1].head = *con;
	}
	else
	{
		config *con_new = (config*)malloc(sizeof(config));
		config_edge *con_edge_new = (config_edge*)malloc(sizeof(config_edge));

		init_config(con_new, (*con)->board_bit, other_color, (*con)->num_current + 1);
		bit_play_move(con_new->board_bit, i, j, color);
		init_config_edge(con_edge_new, POS(i, j), con_new, NULL);

		//�Ӿɸ�ֵ��¸�ֵıߣ�Ҫ���ھɸ���ܵ��ıߵ���������������Ϊ���״��Ѻõ��߷���ǰ��
		if ((*con)->edge_son == NULL)
		{
			(*con)->edge_son = con_edge_new;
		}
		else
		{
			config_edge *temp = (*con)->edge_son->brother;
			while (temp != NULL)
			{
				temp = temp->brother;
			}
			temp = con_edge_new;
		}

		//���µĸ����ӵ���ϣ���������ܵ���ĸ�ֵ���������������ΪҪ�߼ӱ߱Ƚ���û���ظ���
		//��������ģ�Ӧ�ò嵽����ȥ�����Ҳ����ʱ��Ҫ�Ƚ�
		//con_new->next = hash_array[other_color - 1][con_new->num_current].head;
		//hash_array[other_color - 1][con_new->num_current].head = con_new;
		config *temp_config = hash_array[con_new->num_current].head;
		if (temp_config == NULL)
		{
			hash_array[con_new->num_current].head = con_new;
		}
		else
		{
			if (bit_compare(temp_config->board_bit, con_new->board_bit))//�������һ����ϲ�
			{
				con_edge_new->config_son = temp_config;
				con_new = temp_config;
				con_new->times++;
			}
			else{
				while (temp_config->next != NULL)
				{
					if (bit_compare(temp_config->next->board_bit, con_new->board_bit))//�������һ����ϲ�
					{
						con_edge_new->config_son = temp_config->next;
						con_new = temp_config->next;
						con_new->times++;
						break;
					}
					else
					{
						temp_config = temp_config->next;
					}
				}
				if (temp_config->next == NULL)
					temp_config->next = con_new;//����ͷ�˾�ֱ�Ӽ���ĩβ�ͺ���
			}
		}

		//���ǰָ��ָ���µĸ��ȥ
		*con = con_new;
	}
}

/**
* Description:�ַ������Ӻ���
*/
void contact(char *str, const char *str1, const char *str2)
{
	int i, j;

	for (i = 0; str1[i] != '\0'; i++)
	{
		str[i] = str1[i];
	}
	for (j = 0; str2[j] != '\0'; j++)
	{
		str[i + j] = str2[j];
	}
	str[i + j] = '\0';
}


/* ���������� */
void make_chess_manual_tree()
{
	buffer_stack.top = -1;

	FILE *fp;
	char ch; //һ�����ַ�ȥ��
	int i, j; //�����ĵ������
	config *temp_config = NULL;


	char szFilePath[MAX_PATH + 1] = { 0 };
	GetModuleFileNameA(NULL, szFilePath, MAX_PATH);
	char p[MAX_PATH + 1] = { 0 };
	char last[MAX_PATH + 1] = "\\chess_manual\\kogo4.SGF";
	(strrchr(szFilePath, '\\'))[0] = 0; // ɾ���ļ�����ֻ���·���ִ�
	contact(p, szFilePath, last);
//	printf("test\n %s  TESTEND", p);

//	fopen_s(&fp, p, "r");
	fp = fopen(p, "r");
//	printf("after fopen\n");

	if (fp == NULL){
//		printf("\nCannot open file strike any key exit!");
		getch();
		exit(1);
	}
	ch = fgetc(fp);
//	printf("\n before read file\n");
	while (ch != EOF)
	{
		//debug_c(ch);
		switch (ch)
		{
		case '(':
			chess_manual_buffer_push(NULL); //debug_c('(');
			break;
		case ')'://debug_c(')');
			while (chess_manual_buffer_pop() != NULL)
			{

			}
			temp_config = chess_manual_buffer_top(); //
			break;
		case ';':
			ch = fgetc(fp); //debug_c(ch);
			switch (ch)
			{
			case 'B':
				ch = fgetc(fp);
				if (ch == '[')
				{
					ch = fgetc(fp); //debug_c(ch);
					i = 's' - ch;
					if (i > 9)
						i = i - 6;
					ch = fgetc(fp); //debug_c(ch);
					j = ch - 'a';
					if (j > 9)
						j = j - 6;
					//i--;//kogo�ľͲ�Ҫ--��
					//j--;
					refresh_config(&temp_config, i, j, BLACK);
					chess_manual_buffer_push(temp_config);
				}
				else
				{
					//printf("%c ", ch);
				}
				break;
			case 'W':
				ch = fgetc(fp);
				if (ch == '[')
				{
					ch = fgetc(fp); //debug_c(ch);
					i = 's' - ch;
					if (i > 9)
						i = i - 6;
					ch = fgetc(fp); //debug_c(ch);
					j = ch - 'a';
					if (j > 9)
						j = j - 6;
					//i--;
					//j--;
					refresh_config(&temp_config, i, j, WHITE);
					chess_manual_buffer_push(temp_config);
				}
				else
				{
					//printf("%c ", ch);
				}
				break;
			default:
				//printf("%c ", ch);
				break;
			}
			break;
		default:
			break;
			//if (ch != '['&&ch != ']'&&ch != ';' && (ch<'a' || ch>'z'))
			//printf("%c", ch);
		}
		ch = fgetc(fp);
	}

//	printf("after while-read\n");
	fclose(fp);
}

/***********************************************************************************
* ģ�飺ջ�Ĳ���
*
* ��;��
* *������
*
* *void chess_manual_buffer_push(config *push)
*	*��ջ
*
* *config *chess_manual_buffer_pop()
*	*��ջ
*
* *config *chess_manual_buffer_top()
*	*ȡջ��Ԫ��
*
*
***********************************************************************************/

/* ��ջ */
void chess_manual_buffer_push(config *push)
{
	buffer_stack.top++;
	buffer_stack.stack[buffer_stack.top] = push;

	/*int i;
	for (i = 0; i <= buffer_stack.top; ++i)
	printf("%d->", buffer_stack.stack[i]);
	printf("\n");*/
}

/* ��ջ */
config *chess_manual_buffer_pop()
{
	config *pos = buffer_stack.stack[buffer_stack.top];
	buffer_stack.top--;
	/*int i;
	for (i = 0; i <= buffer_stack.top; ++i)
	printf("%d->", buffer_stack.stack[i]);
	printf("\n");*/
	return pos;
}

/* ȡջ��Ԫ�� */
config *chess_manual_buffer_top()
{
	return buffer_stack.stack[buffer_stack.top];
}




