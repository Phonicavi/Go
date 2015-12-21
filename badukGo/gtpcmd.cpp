#include "gtp.h"
// 实现了class GTP中private且void的函数的全部21个
// 主要用于响应GTP协议调用


// 返回协议版本号
void GTP::protocol_version()
{
	response.append(PROTOCOLVERSION);
}
// 返回程序名Baduk
void GTP::name()
{
	response.append(PROGNAME);
}
// 返回程序版本号
void GTP::version()
{
	response.append(PROGVERSION);
}
/**
 * AMOUNTS 是合法指令的总数量 定义在gtp.h的枚举类型中
 * COMMANDS 是指令的字符串数组
 * cmd_args 是string类型的vector
 * 拿着接到的指令和我们认可的合法指令一个一个去比 有就返回"true" 否则返回"false"
 */
void GTP::known_command()
{
	for (int i = 0; i < AMOUNTS; ++i)
	{
		// std::string 的 compare 方法 两字符串相同则返回 0
		if (!cmd_args[0].compare(COMMANDS[i]))
		{
			response.append("true");
		}
	}
	response.append("false");
}
// 列出所有我们认可的合法指令
void GTP::list_commands()
{
	for (int i = 0; i < AMOUNTS; ++i)
	{
		response.append(COMMANDS[i]);
		response.append('\n');
	}
}
// flag 是GTP私有bool成员
// 退出则置为false 不再循环
void GTP::quit()
{
	flag = false;
}
// 先检查这个传入值是否超过了MAXSIZE
// 如果发现不合法 则打印出错误
// 如果没问题、设置成功了 则main_board的set函数设置size 并重启main_go
void GTP::boardsize()
{
	if (cmd_args.size() > 0)
	{
		if (cmd_int_args[0] >= MAXSIZE)
		{
			response[0] = '?';
			response.append("unacceptable size");
		} else {
			main_board.set(cmd_int_args[0]);
			main_go.reset();
		}
	} else {
		response[0] = '?';
		response.append("syntax error");
	}
}
// main_board清空
// main_go重启
void GTP::clear_board()
{
	main_board.clear_board();
	main_go.reset();
}
// main_board 设置贴目数
// 注意set_komi这个函数在Board.h中就已经被实现了
void GTP::komi()
{
	if (cmd_args.size() > 0) {
		main_board.set_komi(cmd_int_args[0]);
	} else {
		response[0] = '?';
		response.append("syntax error");
	}
}
// 执行对面的行棋步骤
void GTP::play()
{
	int side, mv;
	if (cmd_args.size() > 1)
	{
		// play 指令的格式:ex. play w D5
		// 获取的第一个参数是颜色 第二个参数是坐标
		side = char2color(cmd_args[0]);
		mv = char2coordinate(cmd_args[1]);
		if (side > -1 && mv > -1)
		{
			// 合法性检查
			if (main_board.play_move_in_game(mv, side) == -1)
			{
				response[0] = '?';
				response.append("illegal move");
			} else {
				main_go.report_move(mv);
			}
		} else {
			response[0] = '?';
			response.append("invalid side or movement");
		}
	} else {
		response[0] = '?'
		response.append("syntax error");
	}
}
// 真正产生一个决策
void GTP::genmove()
{
	if (cmd_args.size() > 0)
	{
		bool side = char2color(cmd_args[0]);
		if (side != main_board.get_side())
		{
			main_board.play_move(0, !side);
			main_go.report_move(0);
		}
		int mv = main_go.generate_move(early_pass);
		main_board.play_move(mv, side);
		main_go.report_move(mv);
		print_coordinate(mv);
	} else {
		response[0] = '?'
		response.append("syntax error");
	}
}
// 打印棋盘 暂时不打印
void GTP::showboard()
{
	return;
}
// 指定数目让子 暂时没这功能
void GTP::fixed_handicap()
{
	return;
}
// 设置playputs的 估计是设定计算多少盘
// 需要main_go的函数set_max_playouts设定max_playouts
void GTP::level()
{
	if (cmd_int_args.size() > 0 && cmd_int_args[0] > 0)
	{
		main_go.set_max_playouts(10000*cmd_int_args[0]);
	} else {
		response[0] = '?';
		response.append("syntax error");
	}
}
// 时间设定
void GTP::time_settings()
{
	return;
}
// 时间设定 重载
void GTP::time_left()
{
	return;
}
// 封装自己的计分函数 然后根据得分正负打印结果
void GTP::final_score()
{
	std::stringstream auxstream;
	double score = main_go.score(0);

	if (score > 0)
	{
		auxstream << score;
		response.append("B+");
		response.append(auxstream.str());
	} else {
		auxstream << -score;
		response.append("W+");
		response.append(auxstream.str());
	}
}
// 统计棋盘状态 判断死活棋等
void GTP::final_status_list()
{
	if (cmd_args.size() > 0 && !cmd_args[0].compare("dead"))
	{
		std::vector<int> list;
		main_go.score(&list);
		for (std::vector<int>::iterator itr = list.begin(); itr != list.end(); ++itr)
		{
			coord2char(*itr, response, main_board.get_size());
			response.append("\n");
		}
	} else {
		response[0] = '?';
		response.append("syntax error");
	}
}
void GTP::kgs_time_settings()
{
	return;
}
void GTP::kgs_genmove_cleanup()
{
	return;
}
void GTP::unknown_command()
{
	return;
}