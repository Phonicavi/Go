#define _CRT_SECURE_NO_DEPRECATE
#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "gtp.h"
#include "aplus.h"

/* Forward declarations. */
static int gtp_protocol_version(char *s);
static int gtp_name(char *s);
static int gtp_version(char *s);
static int gtp_known_command(char *s);
static int gtp_list_commands(char *s);
static int gtp_quit(char *s);
static int gtp_boardsize(char *s);
static int gtp_clear_board(char *s);
static int gtp_komi(char *s);
static int gtp_fixed_handicap(char *s);
static int gtp_place_free_handicap(char *s);
static int gtp_set_free_handicap(char *s);
static int gtp_play(char *s);
static int gtp_genmove(char *s);
static int gtp_final_score(char *s);
static int gtp_final_status_list(char *s);
static int gtp_showboard(char *s);
static int gtp_raninit(char *s);

/* List of known commands. */
static struct gtp_command commands[] = {
	{ "protocol_version", gtp_protocol_version },
	{ "name", gtp_name },
	{ "version", gtp_version },
	{ "known_command", gtp_known_command },
	{ "list_commands", gtp_list_commands },
	{ "quit", gtp_quit },
	{ "boardsize", gtp_boardsize },
	{ "clear_board", gtp_clear_board },
	{ "komi", gtp_komi },
	{ "fixed_handicap", gtp_fixed_handicap },
	{ "place_free_handicap", gtp_place_free_handicap },
	{ "set_free_handicap", gtp_set_free_handicap },
	{ "play", gtp_play },
	{ "genmove", gtp_genmove },
	{ "final_score", gtp_final_score },
	{ "final_status_list", gtp_final_status_list },
	{ "showboard", gtp_showboard },
	{ "raninit", gtp_raninit },
		{ NULL, NULL }
};

int main(int argc, char **argv) {

	unsigned int random_seed = 1;
	if (argc > 1)
		sscanf(argv[1], "%u", &random_seed);
	srand(random_seed);
	/* Make sure that stdout is not block buffered. */
	setbuf(stdout, NULL);

	/* Inform the GTP utility functions about the initial board size. */
	gtp_internal_set_boardsize(board_size);

	board_array_size = board_size * board_size;

	//srand((unsigned int)time(0));

	/* Initialize the board. */
	init_aplus();

	/* Process GTP commands. */
	gtp_main_loop(commands, stdin, stdout, NULL);

	return 0;
}

/* We are talking version 2 of the protocol. */
static int gtp_protocol_version(char *s) {
	return gtp_success("2");
}

static int gtp_name(char *s){
	return gtp_success(APLUS_NAME);
}

static int gtp_version(char *s){
	return gtp_success(APLUS_VERSION);
}

static int gtp_known_command(char *s){
	int i;
	char command_name[GTP_BUFSIZE];

	/* If no command name supplied, return false (this command never
	 * fails according to specification).
	 */
	if (sscanf(s, "%s", command_name) != 1)
		return gtp_success("false");

	for (i = 0; commands[i].name != NULL; i++)
		if (strcmp(command_name, commands[i].name) == 0)
			return gtp_success("true");

	return gtp_success("false");
}

static int gtp_list_commands(char *s) {
	int i;

	gtp_start_response(GTP_SUCCESS);

	for (i = 0; commands[i].name != NULL; i++)
		gtp_printf("%s\n", commands[i].name);

	gtp_printf("\n");
	return GTP_OK;
}

static int gtp_quit(char *s) {
	gtp_success("");
	return GTP_QUIT;
}

static int gtp_boardsize(char *s) {
	int boardsize;

	if (sscanf(s, "%d", &boardsize) < 1)
		return gtp_failure("boardsize not an integer");

	if (boardsize < MIN_BOARD || boardsize > MAX_BOARD)
		return gtp_failure("unacceptable size");

	board_size = boardsize;
	gtp_internal_set_boardsize(boardsize);
	board_array_size = boardsize * boardsize;
	
	init_aplus();

	return gtp_success("");
}

static int gtp_clear_board(char *s) {
	clear_board(&main_board);
	return gtp_success("");
}

static int gtp_komi(char *s) {
	if (sscanf(s, "%f", &komi) < 1)
		return gtp_failure("komi not a float");

	return gtp_success("");
}

/* Common code for fixed_handicap and place_free_handicap. */
static int place_handicap(char *s, int fixed) {
	int handicap;
	int m, n;
	int first_stone = 1;

	if (!board_empty(&main_board))
		return gtp_failure("board not empty");

	if (sscanf(s, "%d", &handicap) < 1)
		return gtp_failure("handicap not an integer");

	if (handicap < 2)
		return gtp_failure("invalid handicap");

	if (fixed && !valid_fixed_handicap(handicap))
		return gtp_failure("invalid handicap");

	if (fixed)
		place_fixed_handicap(&main_board, handicap);
	else
		place_free_handicap(&main_board, handicap);

	gtp_start_response(GTP_SUCCESS);
	for (m = 0; m < board_size; m++)
		for (n = 0; n < board_size; n++)
			if (main_board.board[POS(m, n)] != EMPTY) {
		if (first_stone)
			first_stone = 0;
		else
			gtp_printf(" ");
		gtp_mprintf("%m", m, n);
			}
	return gtp_finish_response();
}

static int gtp_fixed_handicap(char *s) {
	return place_handicap(s, 1);
}

static int gtp_place_free_handicap(char *s) {
	return place_handicap(s, 0);
}

static int gtp_set_free_handicap(char *s) {
	int i, j;
	int n;
	int handicap = 0;

	if (!board_empty(&main_board))
		return gtp_failure("board not empty");

	while ((n = gtp_decode_coord(s, &i, &j)) > 0) {
		s += n;

		if (main_board.board[POS(i, j)] != EMPTY) {
			clear_board(&main_board);
			return gtp_failure("repeated vertex");
		}

		play_move_game(&main_board, i, j, BLACK);
		handicap++;
	}

	if (sscanf(s, "%*s") != EOF) {
		clear_board(&main_board);
		return gtp_failure("invalid coordinate");
	}

	if (handicap < 2 || handicap >= board_array_size) {
		clear_board(&main_board);
		return gtp_failure("invalid handicap");
	}

	return gtp_success("");
}

static int gtp_play(char *s) {
	int i, j;
	int color = EMPTY;

	if (!gtp_decode_move(s, &color, &i, &j))
		return gtp_failure("invalid color or coordinate");

	if (!legal_move(&main_board, i, j, color))
		return gtp_failure("illegal move");

	play_move_game(&main_board, i, j, color);
	return gtp_success("");
}

static int gtp_raninit(char *s) {
	int color = 1, i, pos;
	//以下为测试时间修改的，想让他随机下20步但实际只下了1步，用showboard可以查看当前棋盘。
	for (i = 0; i < 10; i++)
	{
		pos = generate_random_move(&main_board, color);
		play_move_game(&main_board, I(pos), J(pos), color);
		gtp_start_response(GTP_SUCCESS);
		gtp_mprintf("%m", I(pos), J(pos));
		gtp_finish_response();
		color = 2;
		pos = generate_random_move(&main_board, color);
		play_move_game(&main_board, I(pos), J(pos), color);
		gtp_start_response(GTP_SUCCESS);
		gtp_mprintf("%m", I(pos), J(pos));
		gtp_finish_response();
		gtp_showboard('s');
		color = 1;
	}
	return gtp_finish_response();

}

static int gtp_genmove(char *s) {
	int i, j, pos;
	int color = EMPTY;

	if (!gtp_decode_color(s, &color))
		return gtp_failure("invalid color");

	
		

	pos = generate_move(&main_board, color);
	i = I(pos);
	j = J(pos);
	play_move_game(&main_board, i, j, color);

	gtp_start_response(GTP_SUCCESS);
	gtp_mprintf("%m", i, j);
	return gtp_finish_response();
}

/* Compute final score. We use area scoring since that is the only
 * option that makes sense for this move generation algorithm.
 */
static int gtp_final_score(char *s) {
	float score = komi;
	int i, j;

	compute_final_status_game(&main_board);
	for (i = 0; i < board_size; i++)
		for (j = 0; j < board_size; j++) {
		int status = get_final_status(&main_board, i, j);
		if (status == BLACK_TERRITORY)
			score--;
		else if (status == WHITE_TERRITORY)
			score++;
		else if ((status == ALIVE) ^ (main_board.board[POS(i, j)] == WHITE))
			score--;
		else
			score++;
		}

	if (score > 0.0)
		return gtp_success("W+%3.1f", score);
	if (score < 0.0)
		return gtp_success("B+%3.1f", -score);
	return gtp_success("0");
}

static int gtp_final_status_list(char *s) {
	int n;
	int i, j;
	int status = UNKNOWN;
	char status_string[GTP_BUFSIZE];
	int first_string;

	if (sscanf(s, "%s %n", status_string, &n) != 1)
		return gtp_failure("missing status");

	if (!strcmp(status_string, "alive"))
		status = ALIVE;
	else if (!strcmp(status_string, "dead"))
		status = DEAD;
	else if (!strcmp(status_string, "seki"))
		status = SEKI;
	else
		return gtp_failure("invalid status");

	compute_final_status_game(&main_board);

	gtp_start_response(GTP_SUCCESS);

	first_string = 1;
	for (i = 0; i < board_size; i++)
		for (j = 0; j < board_size; j++)
			if (get_final_status(&main_board, i, j) == status) {
		int k;
		int stonei[MAX_BOARDSIZE];
		int stonej[MAX_BOARDSIZE];
		int num_stones = get_string(&main_board, i, j, stonei, stonej);
		/* Clear the status so we don't find the string again. */
		for (k = 0; k < num_stones; k++)
			set_final_status(&main_board, stonei[k], stonej[k], UNKNOWN);

		if (first_string)
			first_string = 0;
		else
			gtp_printf("\n");

		gtp_print_vertices(num_stones, stonei, stonej);
			}

	return gtp_finish_response();
}

static int gtp_showboard(char *s) {
	int i, j, k;
	int symbols[3] = { '.', 'O', 'X' };

	gtp_start_response(GTP_SUCCESS);
	gtp_printf("\n");

	printf("  ");
	for (k = 0; k < board_size; k++)
		printf(" %c", 'A' + k + (k >= 8));

	for (i = 0; i < board_size; i++) {
		printf("\n%2d", board_size - i);

		for (j = 0; j < board_size; j++)
			printf(" %c", symbols[main_board.board[POS(i, j)]]);

		printf(" %d", board_size - i);
	}
	printf("\n");

	printf("  ");
	for (k = 0; k < board_size; k++)
		printf(" %c", 'A' + k + (k >= 8));

	return gtp_finish_response();
}

/*
* Local Variables:
* tab-width: 8
* c-basic-offset: 2
* End:
*/
