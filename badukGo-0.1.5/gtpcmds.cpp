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
#include "gtp.h"

void GTP::protocol_version()
{
  response.append(PROTOCOLVERSION);
}

void GTP::name()
{
  response.append(PROGNAME);
}

void GTP::version()
{
  response.append(PROGVERSION);
}

void GTP::known_command()
{
  for (int i = 0; i < NCOMMANDS; i++) {
    if (!cmd_args[0].compare(COMMANDS[i]))
      response.append("true");
  }
  response.append("false");
}

void GTP::list_commands()
{
  for (int i = 0; i < NCOMMANDS; i++) {
    response.append(COMMANDS[i]);
    response.append("\n");
  }
}

void GTP::quit()
{
  loop = false;
}

void GTP::boardsize()
{
  if (cmd_args.size() > 0) {
    if (cmd_int_args[0] != main_board.set_size(cmd_int_args[0])) {
      response[0] = '?';
      response.append("unacceptable size");
    } else {
      BadukGo.reset();
    }
  } else {
    response[0] = '?';
    response.append("syntax error");
  }
}

void GTP::clear_board()
{
  main_board.clear();
  BadukGo.reset();
  Current_Step[0] = Current_Step[1] = -1;
}

void GTP::komi()
{
  if (cmd_args.size() > 0) {
    main_board.set_komi(cmd_int_args[0]);
  } else {
    response[0] = '?';
    response.append("syntax error");
  }
}

void GTP::play()
{
  int color, coord;
  if (cmd_args.size() > 1) {
    color = char_to_color(cmd_args[0]);
    coord = char_to_coordinate(cmd_args[1]);

    if (color > -1 && coord > -1) {
      ++Current_Step[color];
      main_board.play_move(coord, color);
        BadukGo.report_move(coord,Current_Step[color]);
        if(board_map[coord] == 1)
            ONE_EXIST = 1;
        else if(board_map[coord] == 2)
            TWO_EXIST = ONE_EXIST = 1;
    } else {
      response[0] = '?';
      response.append("invalid color or coordinate");
    }
  } else {
    response[0] = '?';
    response.append("syntax error");
  }
}

void GTP::kgs_genmove_cleanup()
{
  early_pass = false;
  genmove();
  early_pass = true;
}

void GTP::genmove()
{
#ifdef STD_ERR_PRINT
  std::cerr << "enter genmove" <<std::endl;
#endif
  if(cmd_args.size() > 0){
    bool color = char_to_color(cmd_args[0]);
    if(color != main_board.get_side()){
      main_board.play_move(0, !color);
      BadukGo.report_move(0,Current_Step[!color]);
    }
  #ifdef STD_ERR_PRINT
    std::cerr << "enter generate_move" <<std::endl;
  #endif
    int move = BadukGo.generate_move(early_pass,Current_Step[color]);
  #ifdef STD_ERR_PRINT
    std::cerr << "leave generate_move" <<std::endl;
  #endif
    ++Current_Step[color];
    main_board.play_move(move, color);
  #ifdef STD_ERR_PRINT
    std::cerr << "done play move" <<std::endl;
  #endif 
    BadukGo.report_move(move,Current_Step[color]);
  #ifdef STD_ERR_PRINT
    std::cerr << "done report move" <<std::endl;
  #endif
    print_coordinate(move);
  } else {
    response[0] = '?';
    response.append("syntax error");
  }

}


void GTP::unknown_command()
{
  response[0] = '?';
  response.append("unknown_command");
}

void GTP::showboard()
{
  main_board.print_goban();
}

void GTP::fixed_handicap()
{
  return ;
}

void GTP::level()
{
  if (cmd_int_args.size() > 0 && cmd_int_args[0] > 0) {
    BadukGo.set_playouts(10000*cmd_int_args[0]);
  } else {
    response[0] = '?';
    response.append("syntax error");
  }
}

void GTP::time_settings()
{
  return ;
}

void GTP::kgs_time_settings()
{
  return ;
}

void GTP::time_left()
{
  return ;
}

void GTP::final_score()
{
  std::stringstream auxstream;
  float score = BadukGo.score(0);
  if (score > 0) {
    auxstream << score;
    response.append("B+");
    response.append(auxstream.str());
  } else {
    auxstream << -score;
    response.append("W+");
    response.append(auxstream.str());
  }
}

void GTP::final_status_list()
{
  if (cmd_args.size() > 0 && !cmd_args[0].compare("dead")) {
    std::vector<int> list;
    BadukGo.score(&list);
    //TODO: support 'alive' status.
    for (std::vector<int>::iterator it = list.begin(); it != list.end(); ++it) {
      coord_to_char(*it, response, main_board.get_size());
      response.append("\n");
    }
  } else {
    response[0] = '?';
    response.append("syntax error");
  }
}

void GTP::see_liberty()
{
  if (cmd_args.size() > 0) {
    int point = char_to_coordinate(cmd_args[0]);
    int tmp[169];
    int numlibs = main_board.print_libs_of(point, tmp);
    for (int i = 0; i < numlibs; ++i) {
      coord_to_char(tmp[i], response, main_board.get_size());
      response.append("\t");
    }
  } else {
    response[0] = '?';
    response.append("syntax error");
  }
}

void GTP::see_group()
{
  if(cmd_args.size() > 0){
    int point = char_to_coordinate(cmd_args[0]);
    int tmp[169];
    int numlibs = main_board.print_group_of(point, tmp);
    for (int i = 0; i < numlibs; ++i) {
      coord_to_char(tmp[i], response, main_board.get_size());
      response.append("\t");
    }
  } else {
    response[0] = '?';
    response.append("syntax error");
  }
}

