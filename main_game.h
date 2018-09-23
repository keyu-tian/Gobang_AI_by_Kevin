/**
 * Name: main_game.h
 * Author: Kevin
 * Date: 2018-06-03
 * Description: 游戏主要逻辑 
 */

#ifndef _MAIN_GAME_H
#define _MAIN_GAME_H

#include "base.h"

void AIGame(int mode);							// 小猪佩棋对弈模式 
void FriendsGame(int mode);						// 双人对弈模式 
Point AITurn();									// AI回合

#endif
