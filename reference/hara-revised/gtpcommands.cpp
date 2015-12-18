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
// 实现了class GTP中private且void的函数的全部21个
// 主要用于响应GTP协议调用


// 返回协议版本号
void GTP::protocol_version()
{
  response.append(PROTOCOLVERSION);
}
// 返回程序名Hara
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
 * NCOMMANDS 是合法指令的总数量 定义在gtp.h的枚举类型中
 * COMMANDS 是指令的字符串数组
 * cmd_args 是string类型的vector
 * 拿着接到的指令和我们认可的合法指令一个一个去比 有就返回"true" 否则返回"false"
 */
void GTP::known_command()
{
  for(int i = 0; i < NCOMMANDS; i++){
        // string的compare函数 相同则返回 0
    if(!cmd_args[0].compare(COMMANDS[i])){
      response.append("true");
    }
  }
  response.append("false");
}
// 列出所有我们认可的合法指令
void GTP::list_commands()
{
  for (int i = 0; i < NCOMMANDS; i++){
    response.append(COMMANDS[i]);
    response.append("\n");
  }
}
// loop是GTP私有bool变量
// 退出则置为false 不再循环
void GTP::quit()
{
  loop = false;
}
// goban的set_size函数会检查这个传入值是否超过了MAXSIZE
// 合法才会设成newsize 否则保持原来的size
// 并在设置成功后返回更新了的size
// 如果发现没更新 说明不合法、有问题 则打印出错误
// 如果没问题、设置成功了 则重启engine
void GTP::boardsize()
{
  if(cmd_args.size() > 0){
    if(cmd_int_args[0] != main_goban.set_size(cmd_int_args[0])){
      response[0] = '?';
      response.append("unacceptable size");
    } else {
      go_engine.reset();
    }
  } else {
    response[0] = '?';
    response.append("syntax error");
  }
}
// goban清空
// engine重启
void GTP::clear_board()
{
  main_goban.clear();
  go_engine.reset();
}
// 由goban执行set_komi 设置贴目数
//    注意set_komi这个函数在goban.h中就已经被实现了
void GTP::komi()
{
  if(cmd_args.size() > 0){
    main_goban.set_komi(cmd_int_args[0]);
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
      if(main_goban.play_move(coord, color) == -1){
        response[0] = '?';
        response.append("illegal move");
      } else {
        go_engine.report_move(coord);
      }
    } else {
      response[0] = '?';
      response.append("invalid color or coordinate");
    }
  } else {
    response[0] = '?';
    response.append("syntax error");
  }
}
// 封装了genmove
void GTP::kgs_genmove_cleanup()
{
  early_pass = false;
  genmove();
  early_pass = true;
}
// 真正产生一个决策
void GTP::genmove()
{
  if(cmd_args.size() > 0){
    bool color = char_to_color(cmd_args[0]);
    // 如果color不是自己这边的 则!color是自己这边
    // 会给自己这边做play_move(0, !color)
    if(color != main_goban.get_side()){
      // play_move(int, bool) 在goban.cpp中实现
      // point传0进去的话 会把Goban对象的ko_point置为0
      // 增加一个history 然后什么都不做
      main_goban.play_move(0, !color);
      go_engine.report_move(0);
    }
    // 这里不懂 颜色如果是自己这边的 跳过上面 直接做这里
    //            如果不是 也要做这里
    int move = go_engine.generate_move(early_pass);
    main_goban.play_move(move, color);
    go_engine.report_move(move);
    print_coordinate(move);
  } else {
    response[0] = '?';
    response.append("syntax error");
  }
    
}

// 无脑输出 这个指令我们不认识
void GTP::unknown_command()
{
  response[0] = '?';
  response.append("unknown_command");
}
// 调用print_goban 这个函数在gobanconst.cpp里实现
// 在std::cerr中打印棋盘 打出来的是个命令行里的网格
void GTP::showboard()
{
  main_goban.print_goban();
}
// 指定数目让子
// 调用goban的set_fixed_handicap
void GTP::fixed_handicap()
{
  
  if(cmd_int_args.size() > 0 && cmd_int_args[0] > 1 && cmd_int_args[0] < 10){
    if(main_goban.set_fixed_handicap(cmd_int_args[0]) != cmd_int_args[0]){
    }
  } else {
    response[0] = '?';
    response.append("syntax error");
  }
}
// 设置playputs的 估计是设定计算多少盘
// 调用set_playouts 理解成是搜索深度吧 在engine.cpp中实现
// 设置max_playouts 并将max_time置为了INFINITE
void GTP::level()
{
  if(cmd_int_args.size() > 0 && cmd_int_args[0] > 0){
    go_engine.set_playouts(10000*cmd_int_args[0]);
  } else {
    response[0] = '?';
    response.append("syntax error");
  }
}
// 两种设定时间的 不知道区别是啥
// engine.cpp实现了set_times(int, int, int)和set_times(int, int)两个函数
void GTP::time_settings()
{
  if(cmd_int_args.size() > 2){
    go_engine.set_times(cmd_int_args[0], cmd_int_args[1], cmd_int_args[2]);
  } else {
    response[0] = '?';
    response.append("syntax error");
  }
}

void GTP::kgs_time_settings()
{
  if (cmd_int_args.size() > 3
      && (!cmd_args[0].compare("byoyomi")
          || !cmd_args[0].compare("canadian"))) {
    go_engine.set_times(cmd_int_args[1], cmd_int_args[2], cmd_int_args[3]);
  } else if (cmd_int_args.size() > 1 && !cmd_args[0].compare("absolute")) {
    go_engine.set_times(cmd_int_args[1], 0, 0);
  } else if (cmd_int_args.size() > 0 && !cmd_args[0].compare("none")) {
    go_engine.set_times(30, 0, 0); //To be adjusted.
  } else {
    response[0] = '?';
    response.append("syntax error");
  }
}
// 也是一个时间相关的函数 用的是两个int的那个set_times
void GTP::time_left()
{
  if (cmd_args.size() > 2 && cmd_int_args.size() > 1) {
    go_engine.set_times(cmd_int_args[1], cmd_int_args[2]);
  } else {
    response[0] = '?';
    response.append("syntax error");
  }
}
// 封装自己的计分函数 然后根据得分正负打印结果
// engine.cpp中实现了score函数
void GTP::final_score()
{
  std::stringstream auxstream;
  float score = go_engine.score(0);
  
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
// 统计棋盘状态 判断死活棋等
void GTP::final_status_list()
{
  if (cmd_args.size() > 0 && !cmd_args[0].compare("dead")) {
    std::vector<int> list;
    go_engine.score(&list);
    //TODO: support 'alive' status.
    for(std::vector<int>::iterator it = list.begin(); it != list.end(); ++it){
      coord_to_char(*it, response, main_goban.get_size());
      response.append("\n");
    }
  } else {
    response[0] = '?';
    response.append("syntax error");
  }

}
