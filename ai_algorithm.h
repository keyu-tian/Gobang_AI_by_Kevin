#ifndef _AI_ALGORITHM_H
#define _AI_ALGORITHM_H

#include "base.h"

int MinMaxSearch(int x, int y, int NowPlayer, int depth, int alpha, int beta); // 极大极小值搜索，使用了Alpha-Beta剪枝进行优化

int HeuriSearchChoices(Choice* AvaChoices);     // 启发式搜索函数 生成待遍历可落子点序列

int ChoiceCMP(const void *a, const void *b);    // 用于快排的比较函数
 
Chess RandFirst();                              // 随机生成第一回合的AI落子点

int Win(int x, int y, int NowPlayer);           // 胜负判断

#endif
