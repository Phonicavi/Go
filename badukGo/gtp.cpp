#include <iostream>
#include <sstream>
#include <algorithm>

#include "gtp.h"
#include "base.h"


GTP::GTP():main_go(&main_board)
{
	early_pass = true;
}

int GTP::GTP_mainloop()
{
	flag = true;
	while (flag && getline(std::cin, command_string)) {
		parse(command_string);
		exec();
	}
	return 0;
}

int GTP::parse(const std::string &command_str)
{
	std::stringstream auxstream(command_str);
	std::string auxstring;
	float auxint;
	int id;
	cmd_args.clear();
	cmd_int_args.clear();

	auxstream >> auxstring;
	std::stringstream tempstream(auxstring);

	if (tempstream >> id)
	{
		cmd_id = auxstring;
		cmd_id.append(" ");
		auxstream >> cmd_name;
	} else {
		cmd_id = " ";
		cmd_name = auxstring;
	}

	cmd = string2cmd(cmd_name);

	while (auxstream.good()) {
		auxstream >> auxstring;
		cmd_args.insert(cmd_args.end(), auxstring);

		tempstream.clear();
		tempstream.str(auxstring);
		tempstream >> auxint;
		cmd_int_args.insert(cmd_int_args.end(), auxint);
	}

	return 0;
}

int GTP::exec()
{
	response = "=";
	response.append(cmd_id);

	switch (cmd) {
	case PROTOCOL_VERSION:
		protocol_version();
		break;
	case NAME:
		name();
		break;
	case VERSION:
		version();
		break;
	case KNOWN_COMMAND:
		known_command();
		break;
	case LIST_COMMANDS:
		list_commands();
		break;
	case QUIT:
		quit();
		break;
	case BOARDSIZE:
		boardsize();
		break;
	case CLEAR_BOARD:
		clear_board();
		break;
	case KOMI:
		komi();
		break;
	case PLAY:
		play();
		break;
	case GENMOVE:
		genmove();
		break;
	case SHOWBOARD:
		showboard();
		break;
	case FIXED_HANDICAP:
		fixed_handicap();
		break;
	case LEVEL:
		level();
		break;
	case TIME_SETTINGS:
		time_settings();
		break;
	case TIME_LEFT:
		time_left();
		break;
	case FINAL_SCORE:
		final_score();
		break;
	case FINAL_STATUS_LIST:
		final_status_list();
		break;
	case KGS_TIME_SETTINGS:
		kgs_time_settings();
		break;
	case KGS_GENMOVE_CLEANUP:
		kgs_genmove_cleanup();
		break;
	default:
		unknown_command();
		break;
	}
	response.append("\n\n");
	std::cout << response;

	return 0;
}

int GTP::string2cmd(std::string &size)
{
	transform(size.begin(), size.end(), size.begin(), ::tolower);

	if (!size.compare("white") || !size.compare("w")) return 1;
	else if (!size.compare("black") || !size.compare("b")) return 0;
	else return -1;
}

int GTP::char2coordinate(std::string &coordinate)
{
	int coord;
	transform(coordinate.begin(), coordinate.end(), coordinate.begin(), ::toupper);

	if (!coordinate.compare("PASS")) return 0;

	std::string
}

void GTP::print_coordinate(int coord)
{
  if (coord == -1)
  {
    response.append("resign");
  } else {
    coord2char(coord, response, main_board.get_size());
  }
}

void GTP::perft(int max)
{
	return;
}


