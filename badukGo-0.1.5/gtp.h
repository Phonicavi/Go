/**********************************************************************************************
* Copyright (C) 2016 <BadukGo Project>                                                        *
* All rights reserved                                                                         *
*                                                                                             *
* Permission is hereby granted, free of charge, to any person obtaining a copy of             *
* this software and associated documentation files (the “Software”), to deal in the Software  *
* without restriction, including without limitation the rights to use, copy, modify, merge,   *
* publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons  *
* to whom the Software is furnished to do so, subject to the following conditions:            *
*                                                                                             *
* The above copyright notice and this permission notice shall be included in all copies or    *
* substantial portions of the Software.                                                       *
*                                                                                             *
* THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,         *
* INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR    *
* PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE   *
* FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR        *
* OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER      *
* DEALINGS IN THE SOFTWARE.                                                                   *
**********************************************************************************************/
#ifndef __GTP_H__
#define __GTP_H__


#include <string>
#include <vector>
#include <fstream>
#include <time.h>
#include "Board.h"
#include "Go.h"

#define PROTOCOLVERSION "2"
#define PROGVERSION "0.1.5_release"
#define PROGNAME "BadukGo"
//#define LOG

class GTP {
  private:
  bool loop, early_pass;
  int cmd;
  int Current_Step[2];
  std::string cmd_id;
  std::string cmd_name;
  std::vector<std::string> cmd_args;
  std::vector<float> cmd_int_args;
  std::string command_string;
  std::string response;
#ifdef LOG
  ofstream engine_log;
#endif
  Board main_board;
  Go BadukGo;

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
  void kgs_genmove_cleanup();
  void unknown_command();
  void showboard();
  void fixed_handicap();
  void level();
  void time_settings();
  void kgs_time_settings();
  void time_left();
  void final_score();
  void final_status_list();
  void see_liberty();
  void see_group();

  enum {PROTOCOL_VERSION, NAME, VERSION, KNOWN_COMMAND, LIST_COMMANDS,
        QUIT, BOARDSIZE, CLEAR_BOARD, KOMI, PLAY, GENMOVE, SHOWBOARD,
        FIXED_HANDICAP, LEVEL,TIME_SETTINGS, TIME_LEFT, FINAL_SCORE,
        FINAL_STATUS_LIST, KGS_TIME_SETTINGS, KGS_GENMOVE_CLEANUP, 
        SEE_LIBERTY, SEE_GROUP, NCOMMANDS};

  const std::string COMMANDS[NCOMMANDS] = 
        {"protocol_version","name","version","known_command","list_commands",
         "quit","boardsize","clear_board","komi","play","genmove","showboard",
         "fixed_handicap","level","time_settings","time_left","final_score",
         "final_status_list","kgs-time_settings", "kgs-genmove_cleanup",
         "see_liberty", "see_group"};

  int parse(const std::string&);
  int string_to_cmd(const std::string&);
  int char_to_color(std::string&);
  int char_to_coordinate(std::string&);
  void print_coordinate(int);

public:
  GTP();
  int GTP_loop();
  int exec();
  void perft(int);

};

#endif
