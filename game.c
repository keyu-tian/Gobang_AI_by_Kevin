#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <Windows.h>

#include "game.h"
#include "control.h"
#include "ai_algorithm.h"

const int INF = 9999999;                        // A-B剪枝的初始化极值

const char* OFS = "●";                          // 黑方棋子
const char* DEF = "○";				// 白方棋子
int player, rnd;                                // 记录行棋方和回合
char offensive;                                 // 记录先手方

char __board [LEN+4][LEN+4];
char (*const board)[LEN] =
    (char (*const)[LEN])&__board[2][2];         // 棋盘 

const Point BoardCenter = {7,9};                // 棋盘正中位置坐标（屏幕坐标系）
Point cursor;                                   // 光标位置坐标 （屏幕坐标系）

Chess AIChoice;                                 // 记录最高评分位置 

const char WEIGHT[LEN][LEN] =                   // 棋盘位置权重
{

	{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
	{0,0,0,0,0,0,1,1,1,0,0,0,0,0,0},
	{0,0,1,1,1,1,1,2,1,1,1,1,1,0,0},
	{0,0,1,2,2,2,2,3,2,2,2,2,1,0,0},
	{0,0,1,2,3,3,3,3,3,3,3,2,1,0,0},
	{0,0,1,2,3,6,6,6,6,6,3,2,1,0,0},
	{0,1,1,2,3,6,9,9,9,6,3,2,1,1,0},
	{0,1,2,3,3,6,9,15,9,6,3,3,2,1,0},
	{0,1,1,2,3,6,9,9,9,6,3,2,1,0,0},
	{0,0,1,2,3,6,6,6,6,6,3,2,1,1,0},
	{0,0,1,2,3,3,3,3,3,3,3,2,1,0,0},
	{0,0,1,2,2,2,2,3,2,2,2,2,1,0,0},
	{0,0,1,1,1,1,1,2,1,1,1,1,1,0,0},
	{0,0,0,0,0,0,1,1,1,0,0,0,0,0,0},
	{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}

};


void AIGame(int mode)
{
	int IsFirstRound;
	char key;                                // 记录玩家键盘输入
	Point NowChess;                          // 记录当前落子点

	while (1)
	{
		BoardInit(mode);                 // 棋盘初始化
		IsFirstRound = 1;
		if ( offensive=='a' )
		{
			AITurn();
		}

		while (1)
		{
			key = _getch();          // 利用不回显函数 _getch() 即时获取玩家输入

			if ( IsFirstRound )      // 第一回合的初始化
			{
				cursor = BoardCenter;
				IsFirstRound = 0;
				GotoXY(4*cursor.x, 2*cursor.y);
			}

			if (key == ESC)
			{
				exit(0);
			}
			else if (key == BACKSPACE)	// 按下回退，返回主菜单
			{
				break;
			}
			else if (key == 'n')		// 按下n，New Game
			{
				break;
			}
			else if (key == ' ')		// 按下空格，判断是否可以落子，若可以则落下玩家的棋子
			{
				if (player == PLY)
				{
					NowChess.y = cursor.y - 2;
					NowChess.x = cursor.x ;

					if (!board[NowChess.y][NowChess.x])
					{
						printf(offensive=='p'?OFS:DEF);
						board[NowChess.y][NowChess.x] = PLY;

						rnd++;

						if (Win(NowChess.y, NowChess.x, PLY))	// 玩家胜利
						{
							GotoXY(14, 17);
							printf(" 【 你居然打败了小猪佩棋！ 】 ");

							do
							{
								key = _getch();
							}
							while(key != ESC && key != 'n' && key != BACKSPACE);
							if(key == 'n' || key == BACKSPACE)
								break;
							else if (key==ESC)
								exit(0);

						}
						else
						{
							player = -player;
							NowChess = AITurn();

							if (Win(NowChess.y, NowChess.x, AI))	// AI胜利
							{
								GotoXY(16, 17);
								printf(" 【 你已被小猪佩棋打败！ 】 ");

								do
								{
									key = _getch();
								}
								while (key != ESC && key != 'n' && key != BACKSPACE);
								if( key == 'n' || key == BACKSPACE )
									break;
								else if (key==ESC)
									exit(0);
							}
						}
					}
				}
			}
			else
			{
				key = _getch();  // 获取方向控制的后一个字节
				MoveCursor(key); // 移动光标
			}
		}
		if (key == BACKSPACE)
			break;
	}
}



Point AITurn()
{
	int i,j,r,c,x,y;
	const int CTR = 7;
	Point ret;

	if(rnd==0)
	{
		Sleep ( 250 );
		AIChoice.r = AIChoice.c = CTR;
	}
	else if ( rnd==1 )
	{
		if ( board[CTR][CTR] )
		{
			Sleep ( 250 );
			AIChoice = RandFirst();
		}
		else
		{
			Sleep ( 250 );
			AIChoice.r = AIChoice.c = CTR;
		}
	}
	else MinMaxSearch(0,0,AI,0,-INF,INF);

	r = AIChoice.r;
	c = AIChoice.c;
	board[r][c] = AI;

	player = PLY;		// 回合将轮至玩家
	rnd ++;
	x = abs(cursor.x-c);
	y = abs(cursor.y-2-r);

	if (cursor.x - c>0)
		for (i = 0; i < x; i++)
			MoveCursor(LEFT);
	else for (i = 0; i < x; i++)
			MoveCursor(RIGHT);

	if (cursor.y - 2 - r>0)
		for (i = 0; i < y; i++)
			MoveCursor(UP);
	else for (i = 0; i < y; i++)
			MoveCursor(DOWN);
	printf(offensive=='a'?OFS:DEF);

	ret.x = c;	// 注意坐标和行列数的对应关系，x坐标是列数，y坐标是行数
	ret.y = r;
	return ret;
}



void FriendsGame(int mode)
{
	int IsFirstRound = 0;
	char key;

	while (1)
	{
		BoardInit(mode);                     // 棋盘初始化
		IsFirstRound = 1;

		while (1)
		{

			key = _getch();              // 利用不回显函数 _getch() 即时获取玩家输入

			if ( IsFirstRound )          // 第一回合的初始化
			{
				cursor = BoardCenter;
				IsFirstRound = 0;
				GotoXY(4*cursor.x, 2*cursor.y);
			}

			if (key == ESC)
			{
				exit(0);
			}
			else if (key == BACKSPACE)	// 按下回退，返回主菜单
			{
				break;
			}
			else if (key == 'n')		// 按下n，New Game
			{
				break;
			}
			else if (key == ' ')
			{
				if (player==PLY2)
				{

					if( !board[cursor.y-2][cursor.x] )
					{
						board[cursor.y-2][cursor.x] = PLY2;
						printf("●");
						rnd++;
						if ( Win(cursor.y-2, cursor.x, PLY2) )
						{
							GotoXY(22, 15);
							printf("【  黑子胜利!  】");

							do
							{
								key = _getch();
							}
							while(key != ESC && key != 'n' && key != BACKSPACE);
							if(key == 'n' || key == BACKSPACE)
								break;
							else if (key==ESC)
								exit(0);

						}
						player = PLY;
					}
				}
				else
				{
					if( !board[cursor.y-2][cursor.x] )
					{
						board[cursor.y-2][cursor.x] = PLY;
						printf("○");
						rnd++;
						if( Win(cursor.y-2, cursor.x, PLY) )
						{
							GotoXY(22, 15);
							printf("【  白子胜利!  】");

							do
							{
								key = _getch();
							}
							while(key != ESC && key != 'n' && key != BACKSPACE);
							if(key == 'n' || key == BACKSPACE)
								break;
							else if (key==ESC)
								exit(0);
						}
						player = PLY2;
					}
				}
			}
			else
			{
				key = _getch();
				MoveCursor(key);
			}
		}
		if (key == BACKSPACE)
			break;
	}
}
