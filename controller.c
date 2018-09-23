#include <stdio.h>
#include <string.h>
#include <conio.h>
#include <Windows.h>
#include <time.h>

#include "controller.h"
#include "eval_board.h"

void BoardInit(int mode)		// 初始化棋局 
{
	srand((unsigned)time(NULL));
	memset(board,0,sizeof(board));
	initBitBoard();
	rnd = 0;
	
	system("cls");
	BoardPrint(mode);

	if ( mode==0 )
		player = AI_CHESS;
	else if ( mode==1 )
		player = H1_CHESS;
	else // if ( mode==2 ) 
		player = H2_CHESS;

	cursor.x = 25;
	cursor.y = 18;
	GotoXY(cursor.x, cursor.y);
}

void BoardPrint(int mode)		// 打印棋局主界面 
{
	int i, j;
	if ( mode==0||mode==1 )
		printf("\n\n               【 与  小猪佩棋  对弈中...】\n\n");
	else
		printf("\n\n               【 与  你的小伙伴  对弈中...】\n\n");

	for (j = 0; j<GRID_N; j++)
	{
		for (i = 0; i<GRID_N; i++)
		{
			printf("+   ");
		}
		puts("\n");
	}

	GotoXY(65,8);
	printf("【       操作指令       】");
	GotoXY(65,11);
	printf("【  落子：    空格      】");
	GotoXY(65,14);
	printf("【  光标移动：方向键    】");
	GotoXY(65,17);
	printf("【  新游戏：  n         】");
	GotoXY(65,20);
	printf("【  主菜单：  Backspace 】");
	GotoXY(65,23);
	printf("【  退出：    Esc       】");

	GotoXY(3, 35);printf("                                                                       ");
	GotoXY(3, 35);printf(" AI 评分： %d\t\t估值次数： %lld", 0, 0);
	GotoXY(3, 36);printf("                                                                       ");
	GotoXY(3, 36);printf(" VCF VCT： %lld\t\t剪枝发生： %lld", 0, 0+0+0);
	GotoXY(3, 37);printf("                                                                       ");
	GotoXY(3, 37);printf(" AB 剪枝： %lld\t\t算杀剪枝： %lld\t\t启发式搜索中非安全剪枝： %lld", 0, 0, 0);
			
}

void MenuPrint()		// 打印菜单界面 
{
	printf("\n\n\n\n\n");
	printf("          *****************【 欢迎来到 小猪佩棋的对弈房间 】******************\n\n\n\n\n\n\n\n\n");
	printf("                                 【   模式选择：  】 \n\n");
	printf("                                 【  佩棋先手：0  】 \n\n");
	printf("                                 【  佩棋后手：1  】 \n\n");
	printf("                                 【  双人对弈：2  】 \n\n");
	puts("");
	printf("                                 【   退出：ESC   】 \n\n");
	printf("\n\n\n\n\n");
	GotoXY(52,20);
}

int ModeSelect()		// 游戏模式选择，0--佩棋先手，1--佩棋后手，2--双人对弈 
{
	char mode;
	while(mode = _getch())
	{
		if (mode == ESC)
		{
			exit(0);
		}
		else if (mode == '0')
		{
			offensive = 'a';
			player = AI_CHESS;
			break;
		}
		else if (mode == '1')
		{
			offensive = 'p';
			player = 2;
			break;
		}
		else if (mode == '2')
		{
			offensive = '2';
			player = H2_CHESS;
			break;
		}
	}
	return mode-'0';
}

void GotoXY(short x,short y) // Shell的光标控制 
{
	COORD c= {x,y};
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE),c);
}

void MoveCursor(char key) // 光标移动 
{
	switch (key)
	{

		case UP:
			if ( cursor.y>=2 )
				cursor.y --;
			else
				cursor.y = GRID_N + 1;
			break;

		case DOWN:
			if ( cursor.y<=GRID_N+1 )
				cursor.y ++;
			else
				cursor.y = 2;
			break;

		case LEFT:
			if ( cursor.x>=0 )
				cursor.x --;
			else
				cursor.x = GRID_N-1;
			break;

		case RIGHT:
			if ( cursor.x<GRID_N )
				cursor.x ++;
			else
				cursor.x = 0;
			break;
	}

	GotoXY(4*cursor.x, 2*cursor.y);
}
