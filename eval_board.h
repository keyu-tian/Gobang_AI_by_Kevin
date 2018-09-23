/**
 * Name: eval_board.h
 * Author: Kevin
 * Date: 2018-06-03
 * Description: AI估值 
 */
#ifndef _INC_BOARD_H
#define _INC_BOARD_H

#include "base.h"

//#define REP_MOD 0x3fffff		// 置换表获取哈希下标 

void initHASH();
void initBitBoard();
void updateChessType(const int x, const int y);			// 只 
void takeChess(const int x, const int y);	// 只更新xx_chess数组 
void putChess(const int x, const int y, const Chessid id);	// 只更新xx_chess数组 
int evalBoard();	// 根据all数组只计算分数 
int evalPrior(const int r, const int c, const Chessid id);			// 用于minmax搜索，不依赖于xx_type和xx_all，直接根据xx_chess计算优先级，是原来EvalPrior的优化 
int evalKillPrior(const int r, const int c, const Chessid id);		// 用于算杀搜索，不依赖于xx_type和xx_all，直接根据xx_chess计算优先级 
int killed(Chessid nowplayer, int depth);	// 递归搜索入口的剪枝函数，判断当nowplayer是先手时是否必胜，是返回1，否返回0，必败返回-1 

#endif
