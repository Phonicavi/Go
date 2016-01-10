#pragma once
#include"constants.h"
#include "string.h"
#include "amaf.h"
class GoBoard {
private:
	static int final_status[MAX_BOARD2];
	static int handicap;
	static int diag_i[4];
	static int diag_j[4];
	static int around_i[8];
	static int around_j[8];
	int last_atari[3];


public:
	int stones_on_board[3];
	int last_point;
	int last_point2;
	int empty_points[MAX_BOARD2];
	int empty_points_number;
	String* board[MAX_BOARD2];
	String strings[MAX_BOARD2];
	static float komi;
	static int  I(int pos);
	static int  J(int pos);
	static int POS(int i, int  j);
	static int deltai[4];
	static int deltaj[4];
	static int board_size;
	static int board_size2;
	int ko_i;
	int ko_j;
	int step;
	GoBoard();
	~GoBoard();

	void play_move(int i, int j, int color);
	bool is_true_eye(int point, int color);
	int get_string(int i, int j, int *stonei, int *stonej);
	void show_board();
	static int pass_move(int i, int j);
	int on_board(int i, int j) const;
	int get_board( int i, int j);
	int board_empty();
	void clear_board();
	int suicide(int i, int j, int color);
	int provides_liberty(int ai, int aj, int i, int j, int color);	//some static functions tranfered to not static
	int has_additional_liberty(int i, int j, int libi, int libj);
	int remove_string(int i, int j);
	int same_string(int pos1, int pos2);
	int legal_move(int i, int j, int color);

	int checkLiberty(int i, int j);

	bool available(int i, int j, int color);
	int findALiberty(int i, int j);


	GoBoard * copy_board();
	int generate_legal_moves(int* moves, int color);
	//int autoRun(int color, bool* blackExist, bool* whiteExist);
	int autoRun_fill_the_board(int color, int*simul_len, AmafBoard* tamaf);
	//void calcGame(int *b, int *w, int *bScore, int *wScore);
	void compute_final_status(void);
	int get_final_status(int i, int j);
	void set_final_status(int i, int j, int status);
	int valid_fixed_handicap(int handicap);
	void place_fixed_handicap(int handicap);
	void set_final_status_string(int pos, int status);
	int neighbour_strings(int point, int color, int max_liberties, String *  neighbours[],int index );
	int neighbour_strings( String *group, int color, int max_liberties, String * neighbours[] );
	//int suicideLike(int i, int j, int color);
	int select_and_play(int color);
	int random_legal_move(int color);
	double chinese_count();
	bool is_surrounded(int point, int color);

	////int is_heuristic_available(int color, int last_point);
	int is_star_available(int color, int last_point);
	int is_xiaomu_available(int color, int last_point);
	int is_anti_kakari_available(int color, int last_point);
	int is_anti_yijianjia_available(int color, int last_point);
	int is_anti_dian33_available(int color, int last_point);
	int is_kakari_available(int color, int last_point);
	int relative_point(int pos, int di, int dj);

	bool heavy_policy(int point, int  side);
	//int check_one_Liberty(int i, int j);
	//int find_one_Liberty_for_atari(int i, int j);
	int last_atari_heuristic(int color);
	//void try_to_save_by_eat(int neighbor_i,int neighbor_j,int* saves, int &saves_number);
	//int find_one_Liberty_for_atari2(int bi, int bj, bool*checked);
	int capture_heuristic(int color);
	int save_heuristic(int color);
	int capture_move(int bi, int bj, int color);
	//int gains_liberty(int move, int color);
	int mogo_pattern_heuristic( int color);
	bool match_mogo_pattern(int bi, int bj, int color);
	int random_choose_move(int * moves, int number_moves,int color);
	bool match_hane(int bi, int bj, int color);
	bool match_cut1(int bi, int bj, int color);
	bool match_cut2(int bi, int bj, int color);
	bool match_board_side(int bi, int bj, int color);

	bool is_self_atari(int point, int color);
	int gains_liberty(int move, String* s);
	int add_point(int *points, int points_number, int point);
	int add_string(String * strings[], int strings_number, String* string);
	int total_liberties(int point, int color, int *liberties, int enough, String *exclude);
	bool is_virtual_eye(int point, int color);
	bool is_legal(int point, int color);
	int strings_in_atari(int point, int color, String*strings[], int string_number);
	int find_escape_point(String *group, int escape_points[],int index);
};