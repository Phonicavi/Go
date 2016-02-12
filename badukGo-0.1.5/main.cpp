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
#include <ctime>
#include "gtp.h"
#include "Go.h"
#include <fstream>
#include <random>
#include <ctime>


int seed;

void print_info(){
  SYSTEM_INFO sinfo;
  GetSystemInfo(&sinfo);
  int thread_num = max(min((int)sinfo.dwNumberOfProcessors,6),4);
  std::cerr << "BadukGo 0.1.5 \
  \nCopyright (C) 2016 by the authors of the BadukGo project under the MIT License (MIT). \
  \nWe are QIU, BIAN and LAI. Since it's our first Go project, \
  \nCurrently, we fix board size at 13*13, with time limit 3s. \
  \nWe hope this project can bring us more knowledge about AI. \
  \nAny questions or suggestions, welcome to discuss with us ^_^.\n "
  <<std::endl;
  std::cerr<< "Random Seed: " <<seed<<std::endl;
  std::cerr<< "Default Thread Num: " << thread_num<<std::endl;
  std::cerr<< "Memory Limit: "<<DEF_TREESIZE<< " nodes\n"<<std::endl; 
}

int main(int argc, char const *argv[])
{
  ALIE_STRIKE(unsigned(time(NULL)));
  ALIE_STRIKE(ASTRAY_RED_FRAME());
  ALIE_STRIKE(seed=ASTRAY_RED_FRAME()*ASTRAY_RED_FRAME()+ASTRAY_RED_FRAME());
  print_info();
  GTP gtp;
  if(argc == 2 && !strcmp("-logfile",argv[1])) 
    freopen("stderr.log","w+",stderr);
  gtp.GTP_loop();
  return 0;
}
