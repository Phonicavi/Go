# Go
**Go Engine based on Monte-Carlo using C++ programming language**

---

### Calendar
*2015-Dec.10th:*  

+ Node for Monte-Carlo searching-tree  
	/* 搜索树结点 */  
	`Node.h`  
	`Node.cpp`  

+ Go-board  
	/* 棋盘 */  
	`Board.h`  
	`Board.cpp`  


### Brown-basic
*2015-Dec.10th:*  

+ brown.h  
	包含的定义  

		棋盘规模的范围  
		#define MIN_BOARD 2  
		#define MAX_BOARD 23  

		int board[] 棋盘定义  
		#define EMPTY 0  
		#define WHITE 1  
		#define BLACK 2  

		int final_status[] 最终状态定义  
		#define DEAD 0  
		#define ALIVE 1  
		#define SEKI 2  
		#define WHITE_TERRITORY 3  
		#define BLACK_TERRITORY 4  
		#define UNKNOWN 5  

		贴目数komi 实际棋盘规模board_size  
		float komi  
		int board_size  

+ brown.cpp  
	包含的函数  

		init_brown() // 初始化 清空棋盘数组board 黑白随机下20步  
		clear_board() // 清空棋盘数组  
		board_empty() // 判定全局棋盘是否为空 全空返1 否则返0  
		get_board(i, j) // 返回棋盘(i, j)位置  

		get_string(i, j, *stonei, *stonej) // 返回棋串数目(气相连的棋子数目)  
		pass_move(i, j) // i, j各减1  
		on_board(i, j) // 判断坐标(i, j)是否在棋盘合法范围内  
		legal_move(i, j, color) // 判断坐标(i, j)的move是否合法 合法返1否则返0  
		has_additional_liberty(i, j, libi, libj) // 判断(i, j)位置的棋串是否比(libi, libj)位置的棋串有更多的“气”  
		provides_liberty(ai, aj, i, j, color) // 判断坐标(ai, aj)是否给(i, j)位置的棋子提供了“气”  
		suicide(i, j, color) // 坐标(i, j)的棋子是否是自杀 即move了该位置是死棋, 4个方向上都没有“气”  

		remove_string(i, j) // 提走一个棋串 即将该区域置空  
		same_string(posi, posj) // 判断两个位置是否属于同一个棋串 即在next_stone循环链表里能彼此找到  
  
		play_move(i, i, color) // 在坐标(i, j)布棋 但要注意这里不做合法性检查  
		generate_move(*i, *j, color) // 得出所有可能的位置 记录在moves数组中 并决策生成move  
		set_final_status_string(pos, status) // 设置所有同一棋串的最终状态  

		compute_final_status() // 局终计目 等
		get_final_status(i, j) // 返回坐标(i, j)的final_status  
		set_final_status(i, j, status) // 设置坐标(i, j)的最终状态 区别于set_final_status_string的设置整个相连棋串的状态, 该函数只设置某一位置  
		
		valid_fixed_handicap place_fixed_handicap place_free_handicap  
		... // 让子 等  

+ gtp.h  
	包含的定义  

		最大允许行数 GTP_BUFSIZE // GTP协议通过shell输出的文本命令传递操作  

		GTP 回调函数的返回值  
		#define GTP_QUIT    -1  
		#define GTP_OK       0  
		#define GTP_FATAL    1  

		GTP 命令的执行结果  
		#define GTP_SUCCESS  0  
		#define GTP_FAILURE  1  

		... // 一些函数指针
		... // 封装命令的结构体

+ gtp.cpp  
	包含的函数  


+ interface.cpp  
	调用到的brown.cpp中的函数 14个 // 全部包含在brown.h的申明中  

		init_brown()  
		clear_board()  
		board_empty()  
		get_board(i, j)  

		valid_fixed_handicap(handicap  
		place_fixed_handicap(handicap)  
		place_free_handicap(handicap)  

		legal_move(i, j, color)  

		play_move(i, j, color)  
		generate_move(*i, *j, color)  
		get_string(i, j, stonei, stonej)  
		set_final_status(i, j, status)  

		compute_final_status()  
		get_final_status(i, j)  

	调用到的gtp.cpp中的函数 15个 // 全部包含在gtp.h的申明中  

		gtp_main_loop  
		gtp_internal_set_boardsize  
		gtp_set_vertex_transform_hooks  
		gtp_mprintf  
		gtp_printf  
		gtp_start_response  
		gtp_finish_response  
		gtp_success  
		gtp_failure  
		gtp_panic  
		gtp_decode_color  
		gtp_decode_coord  
		gtp_decode_move  
		gtp_print_vertices  
		gtp_print_vertex  

	主要需要修改brown.cpp中的函数  

		generate_move(*i, *j, color)  
