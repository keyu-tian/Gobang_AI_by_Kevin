#ifndef _AI_EVALUATE_H
#define _AI_EVALUATE_H

#include "base.h"

int  EvalBoard(int x, int y, int NowPlayer);	// 对当前棋局分数进行评估 
int  EvalPrior(int x, int y, int NowPlayer);	// 对某一落子点进行优先级值评估 

void AnalysisVertical  ();						// 分析棋盘纵向15路棋型 
void AnalysisHorizontal();						// 分析棋盘横向15路棋型 
void AnalysisInclined  ();						// 分析棋盘斜向42路棋型 

void ScanLine(const int* const Line, int n);	// 分析线型棋型（核心函数） 


#endif
