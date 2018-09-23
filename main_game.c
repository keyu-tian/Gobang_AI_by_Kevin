#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <Windows.h>
#include <time.h>

#include "main_game.h"
#include "controller.h"
#include "ai_algo.h"
#include "eval_board.h"

const int INF = 9999999;			// A-B剪枝的初始化极值

const char* OFS = "●";				// 黑方棋子
const char* DEF = "○";				// 白方棋子
Chessid player;
int rnd;					// 记录行棋方和回合
char offensive;					// 记录先手方

Chessid board[GRID_N][GRID_N];

const Point BoardCenter(7,9);			// 棋盘正中位置坐标（屏幕坐标系）
Point cursor;					// 光标位置坐标 （屏幕坐标系）
Point AIChoice;					// 记录最高评分位置 

long long D_search_cnt, D_eval_cnt, D_cut_hur, D_cut_ab, D_cut_kill, D_rep_cnt;

void AIGame(int mode)
{
	int IsFirstRound;
	char key;				// 记录玩家键盘输入
	Point NowChess;		    		// 记录当前落子点

	while (1)
	{
		BoardInit(mode);		// 棋盘初始化
		IsFirstRound = 1;

		if ( offensive=='a' )	// 如果AI是先手 
		{
			AITurn();		// AI下第一步棋 
		}

		while (1)
		{
			key = _getch(); 	// 利用不回显函数 _getch() 即时获取玩家输入

			if ( IsFirstRound ) // 第一回合的初始化
			{
				cursor = BoardCenter;
				IsFirstRound = 0;
				GotoXY(4*cursor.x, 2*cursor.y);
			}

			if (key == ESC)			// 按下ESC，退出程序 
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
				if (player == H1_CHESS)
				{
					NowChess.y = cursor.y - 2;
					NowChess.x = cursor.x ;

					if (!board[NowChess.y][NowChess.x])
					{
						printf(offensive=='p'?OFS:DEF);
						
						putChess(NowChess.x, NowChess.y, H1_CHESS);
						updateChessType(NowChess.x, NowChess.y);
//						board[NowChess.y][NowChess.x] = H1_CHESS;

						rnd++;

						if (Win(NowChess.y, NowChess.x, H1_CHESS))	// 如果玩家胜利
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
							player = AI_CHESS;
							NowChess = AITurn();

							if (Win(NowChess.y, NowChess.x, AI_CHESS))	// 如果AI胜利
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
				key = _getch();		// 获取方向控制的后一个字节
				MoveCursor(key);	// 移动光标
			}
		}
		if (key == BACKSPACE)
			break;					// 接力break 
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
		AIChoice.x = AIChoice.y = CTR;
	}
	else if ( rnd==1 )
	{
		if ( board[CTR][CTR] )
		{
			Sleep ( 750 );
			AIChoice = RandFirst();
		}
		else
		{
			Sleep ( 250 );
			AIChoice.x = AIChoice.y = CTR;
		}
	}
	else
	{
		D_eval_cnt = 0;
		D_search_cnt = 0; 
		D_cut_hur = D_cut_ab = D_cut_kill = 0;

		int D_now_score = minMaxSearch(player, 0, -INF, INF);

#ifdef __DEBUG

		GotoXY(3, 35);printf("                                                                                             ");
		GotoXY(3, 35);printf(" AI 评分： %d\t\t估值次数： %lld", D_now_score, D_eval_cnt);
		GotoXY(3, 36);printf("                                                                                             ");
		GotoXY(3, 36);printf(" VCF VCT： %lld\t\t剪枝发生： %lld\t\t",D_search_cnt, D_cut_hur+D_cut_ab+D_cut_kill);
		GotoXY(3, 37);printf("                                                                                             ");
		GotoXY(3, 37);printf(" AB 剪枝： %lld\t\t迭代加深中断： %lld\t\t启发式搜索非安全剪枝： %lld", D_cut_ab, D_cut_kill, D_cut_hur);
		if (D_now_score>=135000/2 || D_now_score<=-135000/2)
		{
			GotoXY(19, 39);printf("                    ");
			GotoXY(19, 39);printf("【 真香警告~  (〃v〃) 】", D_now_score);
		}

#else
		GotoXY(3, 35);printf("                                                                                             ");
		GotoXY(3, 36);printf("                                                                                             ");
		GotoXY(3, 37);printf("                                                                                             ");
		GotoXY(22, 36);printf("未开启Debug监测", D_now_score);
#endif
	}

	r = AIChoice.y;
	c = AIChoice.x;

	putChess(c, r, player);
	updateChessType(c, r);
//	board[r][c] = AI;

	player = H1_CHESS;		// 回合将轮至玩家
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
		BoardInit(mode);		// 棋盘初始化
		IsFirstRound = 1;

		while (1)
		{

			key = _getch(); 	// 利用不回显函数 _getch() 即时获取玩家输入

			if ( IsFirstRound ) // 第一回合的初始化
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
				if (player==H2_CHESS)
				{

					if( !board[cursor.y-2][cursor.x] )
					{
						board[cursor.y-2][cursor.x] = H2_CHESS;
						printf(OFS);
						rnd++;
						if ( Win(cursor.y-2, cursor.x, H2_CHESS) ) // 如果黑子胜利 
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
						player = H1_CHESS;
					}
				}
				else
				{
					if( !board[cursor.y-2][cursor.x] )
					{
						board[cursor.y-2][cursor.x] = H1_CHESS;
						printf(DEF);
						rnd++;
						if( Win(cursor.y-2, cursor.x, H1_CHESS) ) // 如果白子胜利 
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
						player = H2_CHESS;
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
