#ifndef __BOARD_H__
#define __BOARD_H__ 

#include "group.h"
#include <cstring>
#define PASS 0

#define FOOL  0 
#define WISER  1


/* 这个其实并没有什么luan用 */
struct Ppriority
{
	double priority;
	double equivalent;
};


class Amaf{
private:
	int board[MAXSIZE_2+1];
	int size;
	int side;

public:
	Amaf(int _sz){
		size = _sz;
		side = 0;
		memset(board,0,sizeof board);
	}
	~Amaf(){};

	void Amaf_set(bool _side,int _sz,){
		size = _sz;
		side = _side;
		memset(board,0,sizeof board);
	}

	int Amaf_play(int _coord, int _depth){
		if (board[_coord] == 0){
			board[_coord] = side?-_depth:_depth; // white用负数， black 用正数
		}else{
			side = !side;
			return -1;
		}
	}
	/* para是0的话，其实都是返回1.0   para是用来调动态参数的 */
	double Amaf_value(int _coord, int _depth, bool _side, double _para){
		if (_coord == 0) return 0.0;
		int val = side?-board[_coord]:board[_coord];
		if (val >= _depth){
			return 1.0 - _para*val;
		} else return 0;
	}

};

class Board{
private:
	bool side;
	int ko_point;
	int size;
	int size_2;
	double komi;

	Group * belonged_group[MAXSIZE_2+1]; // 棋串的指针表示每个位置分贝属于哪个棋串
	Group * last_atari[2]; // one black, one white

	Group all_groups[MAXSIZE_2+1]; // 棋盘上所有的棋串
	int num_stones_on_board[2]; //  black or white;

	int last_point, last2_point;  // 上一步和上两步



	int four_side[MAXSIZE_2+1][5];
	int four_corner[MAXSIZE_2+1][5];
	int eight_around[MAXSIZE_2+1][8]; // 这个是带空的
	int distance2edge[MAXSIZE_2];

	int within_manhattan[MAXSIZE_2+1][4][20]; 

	PointList<MAXSIZE_2+1> emptys;
	PointList<3*MAXSIZE_2> real_history;

	// zobrist something

	void init();
	void reset();
	void del_empty(int point){emptys.remove(point);} // 将落子的点从空位集当中移走
	/* 非常不愿意这种遍历的remove方式 */




	int drop_stone(int, bool); // 返回落子点
	int tackle_change(int);  // 处理棋盘变化
	void joint_neigh(int, Group*); // 合并
	void remove_neigh(Group *); // 提子




	/* const function */
	int cal_point_libs(int point);// 计算point出的气
	int cal_point_libs(int point, myList &libs); // 计算气，并保存在libs里
	int get_neigh_groups(int point, GroupSet<4> &neigh); // 获取point周围的所有棋串
	/* 获取point周围颜色为_color，气数小于max_libs的棋串 */
	int get_neigh_groups(int point, bool _color, int max_libs,
						 GroupSet<MAXSIZE_2/3> *neigh);

	/* 获取group周围颜色为_color，气数小于max_libs的棋串*/
	int get_neigh_groups(Group *group, bool _color, int max_libs,
						 GroupSet<MAXSIZE_2/3> *neigh);

	/* 获得point周围颜色为_color的棋串的所有棋子个数 */
	int get_neigh_size(int point, bool _color);

	/* 根据neigh来计算point周围有多少颜色为_color的棋串处于打吃状态 */
	int get_neigh_in_atari(int point, bool _color, GroupSet<4> &neigh);

	/* 检查一个空位 假设excul是_color的情况下是否已经被_color的棋子包围了 */
	bool is_surrounded(int point, bool _color, int exculsive = 0);
	/* 检查一个空位 在假设excul是_color的情况下是不是_color的真眼 */
	bool is_true_eye(int point, bool _color, int exculsive = 0);
	/* 检查一个空位 在假设excul是_color的情况下是不是_color的假眼 */
	bool is_fake_eye(int point, bool _color, int exculsive = 0);
	/* 计算分数 */
	bool count_score();
	/* 保存每个位置的得分 */
	void list_score(int li[]);
	int mercy();   // 估算模拟终局形势，如果可以的话，就不要算分


	













public:
	Board(int _sz = 13);
	void clear_board();
	void restore_board();
	void set_komi(double _km){komi = _km;}
	void set(int _sz);
	void shuffle_emptys(){emptys.shuffle_points();}
	int play_move(int mv);   // 模拟棋局时候用的
	int play_move_in_game(int mv, int _color);   // 真正下棋时候用的


	double get_komi(){return komi;}
	bool get_side(){return side;}
	int get_size(){return size;}
	int get_size_2(){return size_2;}
	int get_last_point(){return last_point;}
	int get_last2_point(){return last2_point;}
	bool has_stone(int point){return belonged_group[point] !=0;}
	bool is_legal(int point, bool _color);
	int legal_moves(int moves[]);

	/* Baduk.cpp */
	/* 需要环环的各种补充和扩充 */

	/* 根据method在list当中随机选择一步棋 */
	int choose_by_method(myList &, bool method);
	/* 傻子 */
	bool fool_brain(int mv, bool _side);
	/* 智者 */
	bool wiser_brain(int mv, bool _side);
	/* 随便走 */
	int random_play();
	/* 聪明点走 */
	int wiser_play();

	/* mv这一步能不能增长气 */
	bool gain_libs(int mv, Group* gr);

	/* 
	* 计算mv能给除了excul之外的_color棋串有多少气
	* lim 不为0 时，一旦气数大于lim就返回
	* _lib里面只保存目前局势下气得位置，不保存落子后新产生的气
	 */
	int area_libs(int mv, bool _color, myList* _lib ,int lim, Group* gr);


	/* 以下是和heuristic基本一一对应的 */

	// trick
	int get_atari_escapes(Group* gr, myList &);
	int cal_create_eyes(int, bool);

	int get_last_atari();
	void get_nakade_list(int lap, myList & );
	void get_capture_list(int lap, myList &);
	void get_pattern_list(int lap, myList &);
	void get_save_list(int lap, myList &);






	//judgement 

	bool is_self_atari(int mv, bool _side);
	bool is_ladder(int mv, bool _side);
	bool is_stones_around(int mv, int max_dist);
	bool is_bad_atari(int mv, bool_side);
	bool is_nakade_shape(int ,bool);
	bool is_match_pattern(int, bool);




	




};



#endif