/**
 * Name: controller.h
 * Author: Kevin
 * Date: 2018-06-03
 * Description: 游戏控制
 */

#ifndef _CONTROLLER_H
#define _CONTROLLER_H

#include "base.h"

enum KEYBOARD_KEY
{
    ESC   =   27,
    UP    = 0x48,
    DOWN  = 0x50,
    LEFT  = 0x4B,
    RIGHT = 0x4D,
    BACKSPACE= 8,
};

extern Point cursor;			// 坐标 
extern Chessid player;
extern int rnd;			// 行棋方和回合 
extern char offensive;			// 先手方

int  ModeSelect();				// 游戏模式选择 
void BoardInit(int mode);		// 棋局初始化 
void BoardPrint(int mode);		// 打印棋局主界面 
void MenuPrint();				// 打印菜单主界面 
void GotoXY(short x,short y);	// Shell光标控制 
void MoveCursor(char key);		// 光标移动

#endif
