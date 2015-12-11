#ifndef _APLUS_H_
#define _APLUS_H_

#include "board.h"

//3.0 only pattern
//6.0 打开了救子、提气、开局四步，增加20步以内用棋谱
//6.1 棋谱的匹配做了一点改进，能正反两种颜色比较，由原来占四个角变为占两个角
//6.2 换了一个更全一点的棋谱kogo，对比两个对角，右下角的起点要翻个方向
//6.3 棋谱改进：生成棋谱的部分原来错了，对于棋谱的不好的棋要舍弃；开局四步：原来那个是不好的下法
//6.4 加上一个做眼的小步骤，其实和吃子差不多
//6.5 6.4的基础上再加了一点点，即四边上的
//6.6 pull了，与7.1合并
//6.7 棋谱只在前10步
//6.8 返回棋谱中匹配的全部位置，供uct使用，目前输出次数最多的位置
//6.9 避免填自己的眼（在uct前面加的，看到过一次比赛是因为救子导致的填眼，不知道这样好不好，感觉眼的判断还得另外来）
//6.10 与7.3合并
//6.11 7.7之后，uct计算盘数变为计算分数
//6.12 uct里用了置换表，aplus里面的uct以外的方法都注释起来了

//7 加了一个优先吃字
//7.1 周边棋子加起来被吃掉的气大于2
//7.2 打开uct 里面的pattern 救子的优先级作为最高
//7.3 与6.9合并 注释掉棋谱 pattern在中盘的时候在uct中起作用 10-100步之间起作用
//7.4 1/3的概率下提前救子和吃字和提气
//7.5 继7.4 修改random
//7.6 继7.5 打开棋谱
//7.7 继7.5 修改random 20次

//test 1 测试那个illegal和棋谱的关系
//test uct 测试uct和illegal的关系
//test qipu 测试qipu和illegal的关系

//8 uct+qipu+pattern+random优化

//9 修正的8的错误(pattern,置换表,布局)
//9.1 和球的版本合并（球那个代码太旧了），纯uct（pattern、置换表、布局、棋谱、其它trick已关）
//9.2 继承9.1，纯uct+置换表
//9.3 换了uct数据结构的纯uct
//9.4 换uct数据结构之前的纯uct
//9.5 换了uct数据结构的uct+置换表
//9.6 改正了9.5的置换表错误，换了uct数据结构的uct+置换表，uct稍微优化了一下
//9.7 9.6基础上（稍微优化过的uct）没有置换表的版本
//9.8 纯uct+开局4步+“棋谱+吃子救子紧气+真眼+连接切断+防填眼”组成的排好序的候选点（没有置换表） 
//9.9 继承9.8 修复illegal
//9.10 继承9.9 修复trick里面的的气

//10 继承9.1 打开pattern 全程都用pattern
//10.1 继承9.1 打开pattern 中场使用pattern
//10.2 继承9.1 只用围棋定式

//t2.4 纯uct用score作为wins + 其它tricks
//t2.2.4 纯uct用score作为wins + 其它tricks + pattern中场
//1.4 
//1.3.4
//1.1.3.4
//t1 纯uct_pure
//1.3.4-boundary

//11 全打开
//11_1 关掉uct里的策略，用伪气
//11_2 关掉uct里的策略，用伪气
//11_3 trick全打开，用真气，10步以上uct里面的模拟要加吃子救子和pattern
//11_4 trick全打开，用真气，20步以上uct里面的模拟要加吃子救子和pattern，10~40步忽略四周都没有子的点
//11_5 关掉uct里的策略，用伪气，10~40步忽略四周都没有子的点
//11_6 11_4基础上，吃子救子全程
//11_7 11_5基础上，吃子救子全程

//Aplus_whole	全部代码 完成aplus_11_7的功能

#define APLUS_NAME "Aplus_whole"
#define APLUS_VERSION "13"

#define PATH_CHESS_MANUAL "E:\\chess_manual\\kogo4.SGF"
//#define PATH_CHESS_MANUAL "C:\\Users\\qiaomai\\Documents\\gnuaplus_new\\chess_manual\\kogo4.SGF"
//#define PATH_CHESS_MANUAL "C:\\Users\\happyqiaomai\\Documents\\GnuAplus\\chess_manual\\kogo4.SGF"
//#define PATH_CHESS_MANUAL "W:\\qxt\\computer\\AI\\teamwork\\go\\chess_manual\\kogo4.SGF"
//#define PATH_CHESS_MANUAL "E:\\Users\\daniyuu\\Visual Studio 2013\\source\\gnuaplus\\\chess_manual\\kogo4.SGF"
//#define PATH_CHESS_MANUAL "F:\\人工智能\\final_project\\GnuAplus\\bitbucket\\chess_manual\\kogo4.SGF"
//#define PATH_CHESS_MANUAL "E:\\Users\\yueying\\visual studio 2013\\Source\\gnuaplus\\chess_manual\\kogo4.SGF"

//分段
#define STEPS_START_END 3 //开局要走多少步固定位置
#define STEPS_MANUAL_END 10 //到多少步为止按照棋谱来走
#define STEPS_EAT_SAVE_END 140 //吃子和救子的步数限制
#define STEPS_CONNECT_CUT_END 40 //连接和切断的步数限制
#define STEPS_MIDDLE_START 50	//中场开始
#define STEPS_MIDDLE_END 100	//中场结束
#define MAX_LEGGAL_RANDOM 20	//最大random hit尝试步数
#define STEPS_BOUNDARY_TWO 10 //多少步以内忽略棋牌最外圈两层点
#define STEPS_BOUNDARY_ONE 20 //多少步以内忽略棋牌最外圈一层点

//在排序之前的，result_to_uct[i]表示pos=i的位置的权重
//不合法：-1
//最普通的点：0
int result_to_uct[MAX_BOARDSIZE];
int result_to_uct_pos[MAX_BOARDSIZE];

void init_aplus(void);
int generate_random_move(board_status *bs, intersection color);
void place_free_handicap(board_status *bs, int handicap);
int generate_move(board_status *bs, intersection color);
int sim_for_string(board_status *bs, int pos, intersection color);
int divide_result(int low, int high);
void quick_sort_for_result(int low, int high);

#endif