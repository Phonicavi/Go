/***************************************************************************************
* Copyright (c) 2014, Antonio Garro.                                                   *
* All rights reserved                                                                  *
*                                                                                      *
* Redistribution and use in source and binary forms, with or without modification, are *
* permitted provided that the following conditions are met:                            *
*                                                                                      *
* 1. Redistributions of source code must retain the above copyright notice, this list  *
* of conditions and the following disclaimer.                                          *
*                                                                                      *
* 2. Redistributions in binary form must reproduce the above copyright notice, this    *
* list of conditions and the following disclaimer in the documentation and/or other    *
* materials provided with the distribution.                                            *
*                                                                                      *
* 3. Neither the name of the copyright holder nor the names of its contributors may be *
* used to endorse or promote products derived from this software without specific      *
* prior written permission.                                                            *
*                                                                                      * 
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"          *
* AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE            *
* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE           *
* ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE            *
* LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL    *
* DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR           *
* SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER           *
* CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR     *
* TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF        *
* THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.                    *
***************************************************************************************/
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
  for(int i = 0; i < NCOMMANDS; i++){
    if(!cmd_args[0].compare(COMMANDS[i])){
      response.append("true");
    }
  }
  response.append("false");
}

void GTP::list_commands()
{
  for (int i = 0; i < NCOMMANDS; i++){
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
  if(cmd_args.size() > 0){
    if(cmd_int_args[0] != main_board.set_size(cmd_int_args[0])){
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
}

void GTP::komi()
{
  if(cmd_args.size() > 0){
    main_board.set_komi(cmd_int_args[0]);
  } else {
    response[0] = '?';
    response.append("syntax error");
  }
}

void GTP::play()
{
  int color, coord;
  if(cmd_args.size() > 1){
    color = char_to_color(cmd_args[0]);
    coord = char_to_coordinate(cmd_args[1]);
    
    if(color > -1 && coord > -1){
      //Check legality.
      // if(){
      //   response[0] = '?';
      //   response.append("illegal move");
      // } else {
      main_board.play_move(coord, color);
        BadukGo.report_move(coord);
      // }
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
  if(cmd_args.size() > 0){
    bool color = char_to_color(cmd_args[0]);
    if(color != main_board.get_side()){
      main_board.play_move(0, !color);
      BadukGo.report_move(0);
    }
    int move = BadukGo.generate_move(early_pass);
    main_board.play_move(move, color);
    BadukGo.report_move(move);
    print_coordinate(move);
    // main_board.record_goban();
    // showboard();
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


  // std::cerr<<main_board.print_goban_gtp();
}

void GTP::fixed_handicap()
{
  return;
  
  // if(cmd_int_args.size() > 0 && cmd_int_args[0] > 1 && cmd_int_args[0] < 10){
  //   if(main_board.set_fixed_handicap(cmd_int_args[0]) != cmd_int_args[0]){
  //   }
  // } else {
  //   response[0] = '?';
  //   response.append("syntax error");
  // }
}

void GTP::level()
{
  if(cmd_int_args.size() > 0 && cmd_int_args[0] > 0){
    BadukGo.set_playouts(10000*cmd_int_args[0]);
  } else {
    response[0] = '?';
    response.append("syntax error");
  }
}

void GTP::time_settings()
{
  // if(cmd_int_args.size() > 2){
  //   BadukGo.set_times(cmd_int_args[0], cmd_int_args[1], cmd_int_args[2]);
  // } else {
  //   response[0] = '?';
  //   response.append("syntax error");
  // }

  return ;
}

void GTP::kgs_time_settings()
{
  // if (cmd_int_args.size() > 3
  //     && (!cmd_args[0].compare("byoyomi")
  //         || !cmd_args[0].compare("canadian"))) {
  //   BadukGo.set_times(cmd_int_args[1], cmd_int_args[2], cmd_int_args[3]);
  // } else if (cmd_int_args.size() > 1 && !cmd_args[0].compare("absolute")) {
  //   BadukGo.set_times(cmd_int_args[1], 0, 0);
  // } else if (cmd_int_args.size() > 0 && !cmd_args[0].compare("none")) {
  //   BadukGo.set_times(30, 0, 0); //To be adjusted.
  // } else {
  //   response[0] = '?';
  //   response.append("syntax error");
  // }
  return ;
}

void GTP::time_left()
{
  // if (cmd_args.size() > 2 && cmd_int_args.size() > 1) {
  //   BadukGo.set_times(cmd_int_args[1], cmd_int_args[2]);
  // } else {
  //   response[0] = '?';
  //   response.append("syntax error");
  // }
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
    for(std::vector<int>::iterator it = list.begin(); it != list.end(); ++it){
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
  if(cmd_args.size() > 0){
  	int point = char_to_coordinate(cmd_args[0]);
  	int tmp[169];
  	int numlibs = main_board.print_libs_of(point, tmp);
    for (int i = 0; i < numlibs; ++i)
    {
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
    for (int i = 0; i < numlibs; ++i)
    {
      coord_to_char(tmp[i], response, main_board.get_size());
      response.append("\t");
    }
  } else {
    response[0] = '?';
    response.append("syntax error");
  }
}





