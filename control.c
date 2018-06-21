#include <stdio.h>
#include <string.h>
#include <conio.h>
#include <Windows.h>
#include "control.h"


void BoardInit(int mode)		// 初始化棋局 
{
	int i, j;
	rnd = 0;
	memset(__board,0,sizeof(__board));
	system("cls");
	BoardPrint(mode);

	if ( mode==0 )
		player = AI;
	else if ( mode==1 )
		player = PLY;
	else // if ( mode==2 )
		player = PLY2;
	
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

	for (j=0; j<LEN; j++)
	{
		for (i=0; i<LEN; i++)
		{
			printf("+   ");
		}
		puts("\n");
	}

	GotoXY(65,6);
	printf("【       操作指令       】");
	GotoXY(65,9);
	printf("【  落子：    空格      】");
	GotoXY(65,12);
	printf("【  光标移动：方向键    】");
	GotoXY(65,15);
	printf("【  新游戏：  n         】");
	GotoXY(65,18);
	printf("【  主菜单：  Backspace 】");
	
}

void MenuPrint()		// 打印菜单界面 
{
	printf("\n\n\n\n\n");
	printf("          *****************【 欢迎来到 小猪佩棋的对弈房间 】******************\n\n\n\n\n\n\n\n\n");
	printf("                                 【   模式选择：  】 \n\n");
	printf("                                 【  佩棋先手：0  】 \n\n");
	printf("                                 【  佩棋后手：1  】 \n\n");
	printf("                                 【  双人对弈：2  】 \n\n");
	printf("                                 【   退出：ESC   】 \n\n");
	printf("\n\n\n\n\n");
	GotoXY(52,20);
}

int ModeSelect()		// 游戏模式选择，0--佩棋先手，1--佩棋后手，2--双人对弈 ，ESC--退出
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
			player = AI;
			break;
		}
		else if (mode == '1')
		{
			offensive = 'p';
			player = PLY;
			break;
		}
		else if (mode == '2')
		{
			player = PLY2;
			break;
		}
	}
	return mode - '0';
}

void GotoXY(short x,short y)    // Shell的光标控制 
{
	COORD c= {x,y};
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE),c);
}

void MoveCursor(char key)       // 光标移动 
{
	switch (key)
	{

		case UP:
			if ( cursor.y>=2 )
				cursor.y --;
			else
				cursor.y = LEN + 1;
			break;

		case DOWN:
			if ( cursor.y<=LEN+1 )
				cursor.y ++;
			else
				cursor.y = 2;
			break;

		case LEFT:
			if ( cursor.x>=0 )
				cursor.x --;
			else
				cursor.x = LEN-1;
			break;

		case RIGHT:
			if ( cursor.x<LEN )
				cursor.x ++;
			else
				cursor.x = 0;
			break;
	}

	GotoXY(4*cursor.x, 2*cursor.y);		// 按比例放大输出字符
}
