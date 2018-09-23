/**
 * Name: base.h
 * Author: Kevin
 * Date: 2018-06-03
 * Description: 基础声明
 */

#ifndef _BASE_H
#define _BASE_H

#include <stdlib.h>


#define __DEBUG


#define GRID_N 15
#define GRID_DN 19
#define CHESS_TYPE_N 17

#define GET_L(x, y) x - y + 9
#define GET_R(x, y) x + y - 5

enum : char { NO_PLY = 0, AI_CHESS = 1, H1_CHESS = 2, H2_CHESS = 3 };

typedef char Chessid;

extern Chessid board[GRID_N][GRID_N];

extern long long D_search_cnt, D_eval_cnt, D_cut_hur, D_cut_ab, D_cut_kill;
//extern const int WEIGHT[GRID_N][GRID_N];

struct Point
{
	int x;
	int y;
	Point(void) { }
	Point(int x, int y) : x(x), y(y) { }
};

extern Point AIChoice;

struct Choice
{
	int r;
	int c;
	int prior;
};


#endif
