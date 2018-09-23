/**
 * Name: ai_algo.h
 * Author: Kevin
 * Date: 2018-06-03
 * Description: AI关键算法
 */

#ifndef _AI_ALGO_H
#define _AI_ALGO_H

#include "base.h"

int minMaxSearch(Chessid NowPlayer, int depth, int alpha, int beta); // 极大极小值搜索，使用了Alpha-Beta剪枝进行优化 

int killSearch(Chessid NowPlayer, int depth, int alpha, int beta); // 极大极小值搜索，使用了Alpha-Beta剪枝进行优化 

int heuriSearchChoices(Choice *AvaChoices);			// 启发式搜索函数 生成待遍历可落子点序列 

int killSearchChoices(Choice *AvaChoices);			// 启发式搜索函数 生成待遍历可落子点序列 

void quickSort(Choice *a, int n);
void merge(Choice *a, int n_left, int n_right);
void mergeSort(Choice *a, int n);
 
Point RandFirst();									// 随机生成第一回合的AI落子点

int Win(int x, int y, Chessid NowPlayer);				// 胜负判断 

#endif
