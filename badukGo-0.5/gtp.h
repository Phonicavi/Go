#ifndef __GTP__
#define __GTP__

#include <string>
#include <vector>
#include <fstream>
#include <time.h>
#include "Board.h"
#include "Go.h"

// GTP协议版本 2.0
// 程序版本 1.0
// 程序名字 Baduk
// 20条支持的指令
#define PROTOCOLVERSION "2"
#define PROGVERSION "1.0"
#define PROGNAME "Baduk"
#define AMOUNTS 20


class GTP {// GTP是一个协议实现类
// GTP包含了决策的实现 对决策结果的标准化、协议化封装
private:
	enum CommandSet
	{
		PROTOCOL_VERSION, // id: '0'
		NAME,
		VERSION,
		KNOWN_COMMAND,
		LIST_COMMANDS,
		QUIT,
		BOARDSIZE,
		CLEAR_BOARD,
		KOMI,
		PLAY,
		GENMOVE,
		SHOWBOARD,
		FIXED_HANDICAP,
		LEVEL,
		TIME_SETTINGS,
		TIME_LEFT,
		FINAL_SCORE,
		FINAL_STATUS_LIST,
		KGS_TIME_SETTINGS,
		KGS_GENMOVE_CLEANUP // id: '19', just 20 commands available
	};
	bool flag, early_pass;
	int cmd;
	std::string cmd_id;
	std::string cmd_name;
	std::vector<std::string> cmd_args;
	std::vector<float> cmd_int_args;
	std::string command_string;
	std::string response;
	// Board: 棋盘
	// Go: 决策
	Board main_board;
	Go main_go;
	// CommandSet 对应的指令匹配字符串
	const std:string COMMANDS[AMOUNTS] = {
		"protocol_version",
		"name",
		"version",
		"known_command",
		"list_commands",
		"quit",
		"boardsize",
		"clear_board",
		"komi",
		"play",
		"genmove",
		"showboard",
		"fixed_handicap",
		"level",
		"time_settings",
		"time_left",
		"final_score",
		"final_status_list",
		"kgs-time_settings",
		"kgs-genmove_cleanup"
	};
public:
	GTP();
	int GTP_mainloop();
	int exec();
	void perft(int);
	~GTP();

private:
	// functions for GTP-Commands
	void protocol_version();
	void name();
	void version();
	void known_command();
	void list_commands();
	void quit();
	void boardsize();
	void clear_board();
	void komi();
	void play();
	void genmove();
	void showboard();
	void fixed_handicap();
	void level();
	void time_settings();
	void time_left();
	void final_score();
	void final_status_list();
	void kgs_time_settings();
	void kgs_genmove_cleanup();
	void unknown_command();

	// functions for string parse
	int parse(const std::string &);
	int string2cmd(const std::string &);
	int char2color(std::string &);
	int char2coordinate(std::string &);
	void print_coordinate(int);
};

#endif
