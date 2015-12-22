#pragma once
#define OTHER_COLOR(color) (WHITE + BLACK - (color))
#define EMPTY 0
#define WHITE 1
#define BLACK 2
#define THREAD_NUM 4

#define MIN_BOARD 2
#define MAX_BOARD 23



#define IMPACT 32
#define IMPACTDIS 5
#define AIMOVEMAX 10
#define SAMECOLOR 4
#define DIFFERENTCOLOR 2
#define STARTMEDIAN 5
#define CORNERSIZE 5
#define MINMAXRANGE 3
#define BLACKEDGE 4
#define WHITEEDGE -BLACKEDGE
#define MEDIANMINMAX 50
#define TRYTIME 10
#define MAXSTEP 60
#define TIMELIMIT 2
#define MAXGAMES 2250
#define MONTECARLORANGE 13
#define CLEARTIME 10
#define VERSION_STRING "3.1"
#define UNKNOWN 5
#define DEAD 0
#define ALIVE 1
#define SEKI 2
#define WHITE_TERRITORY 3
#define BLACK_TERRITORY 4
#define PRECHECKRANGE 2
#define CALCGAMESHIFT 75
#define MAX_BEGINING 30

#define MAXTIME 3*CLOCKS_PER_SEC