#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include "ai_algo.h"
#include "eval_board.h"

int BASE_DEPTH = 4;
int KILL_DEPTH = 7;
#define KILL_NUM   8
#define WinScore   135000	// 与 C5Score 保持同步
#define KILL_PRIOR 512		// 在evalPrior函数中至少有两个冲四活三 

#define swapChoice(x, y) do{Choice tp=x; x=y; y=tp;}while(0)
#define ifSwapChoice(x, y) if(x.prior < y.prior) swapChoice(x, y)

Choice tp[GRID_N];

int killSearch(Chessid NowPlayer, int depth, int alpha, int beta)
{
#ifdef __DEBUG
	D_search_cnt++;
#endif

	int score, NumOfAva;

	int flag = killed(NowPlayer, depth);
	if (flag)
	{
#ifdef __DEBUG
		D_cut_kill++;
#endif
		return flag;
	}

	if ( depth>=KILL_DEPTH )
	{
		return evalBoard();
	}

	Choice AvaChoices[GRID_N*GRID_N];
	NumOfAva = killSearchChoices( AvaChoices );
	if (!NumOfAva)
	{
		return evalBoard();
	}

	if ( NowPlayer==AI_CHESS )
	{
		int k, r, c;

		for (k=0; k<NumOfAva; k++)   	
		{
			r = AvaChoices[k].r;
			c = AvaChoices[k].c;

			putChess(c, r, AI_CHESS);
			updateChessType(c, r);
//			board[r][c] = AI;
			score = killSearch(H1_CHESS, depth+1, alpha, beta); // 递归计算下一层分数 
//			board[r][c] = 0;
			takeChess(c, r);
			updateChessType(c, r);
			
			if (score > alpha)			// 对于AI，更新极大值 
			{
				alpha = score;
			}
			if (alpha >= beta)   		// Alpha-剪枝
			{
#ifdef __DEBUG
				D_cut_ab++;
#endif
				return alpha;
			}
		}
		return alpha;
	}
	else // if ( NowPlayer==H1_CHESS )
	{
		int r, c, k;

		for (k=0; k<NumOfAva; k++)
		{
			r = AvaChoices[k].r;
			c = AvaChoices[k].c;

			putChess(c, r, H1_CHESS);
			updateChessType(c, r);
//			board[r][c] = H1_CHESS;			// 临时落子 
			score = killSearch(AI_CHESS, depth+1, alpha, beta); // 递归计算下一层分数 
			takeChess(c, r);
			updateChessType(c, r);
//			board[r][c] = 0;			// 撤子

			if (score < beta)			// 对于玩家，更新极小值
			{
				beta = score ;
			}
			if (alpha >= beta)   		// Beta-剪枝
			{
#ifdef __DEBUG
				D_cut_ab++;
#endif
				return beta ;
			}
		}
		return beta;
	}
}


//  利用极大极小搜索算法 遍历博弈树
//  并利用Alpha-Beta剪枝算法优化
int minMaxSearch(Chessid NowPlayer, int depth, int alpha, int beta)
{
	if ( depth>=BASE_DEPTH )
	{
//		reCount();
//		return EvalBoard();
		return killSearch(NowPlayer, depth, alpha, beta);
	}

	int score, NumOfAva;
	int flag;

	if ( depth > 0 )		// depth必须>0，不然无法获取胜利点的坐标就返回了 
	{
		flag = killed(NowPlayer, depth);
		if (flag)
		{
#ifdef __DEBUG
			D_cut_kill++;
#endif
			return flag;
		}
	}
	
	Choice AvaChoices[GRID_N*GRID_N];
	NumOfAva = heuriSearchChoices( AvaChoices );

	if ( NowPlayer==AI_CHESS )
	{
		int k, r, c;

		for (k=0; k<NumOfAva; k++)   	
		{
			r = AvaChoices[k].r;
			c = AvaChoices[k].c;

			putChess(c, r, AI_CHESS);
			updateChessType(c, r);
//			board[r][c] = AI;
			score = minMaxSearch(H1_CHESS, depth+1, alpha, beta); // 递归计算下一层分数 
//			board[r][c] = 0;
			takeChess(c, r);
			updateChessType(c, r);
			
			if (score > alpha)			// 对于AI，更新极大值 
			{
				if (!depth)
				{
					AIChoice.x = c;
					AIChoice.y = r;
				}
				alpha = score;
			}
			if (alpha >= beta)   		// Alpha-剪枝
			{
#ifdef __DEBUG
				D_cut_ab++;
#endif
				return alpha;
			}
		}
		return alpha;
	}
	else // if ( NowPlayer==H1_CHESS )
	{
		int r, c, k;

		for (k=0; k<NumOfAva; k++)
		{
			r = AvaChoices[k].r;
			c = AvaChoices[k].c;

			putChess(c, r, H1_CHESS);
			updateChessType(c, r);
//			board[r][c] = H1_CHESS;			// 临时落子 
			score = minMaxSearch(AI_CHESS, depth+1, alpha, beta); // 递归计算下一层分数 
			takeChess(c, r);
			updateChessType(c, r);
//			board[r][c] = 0;			// 撤子

			if (score < beta)			// 对于玩家，更新极小值
			{
				beta = score ;
			}
			if (alpha >= beta)   		// Beta-剪枝
			{
#ifdef __DEBUG
				D_cut_ab++;
#endif
				return beta ;
			}
		}
		return beta;
	}
}


//  利用启发式搜索优化Alpha-Beta剪枝
int heuriSearchChoices( Choice* AvaChoices )
{
	int i, j, k, l, num, KillNum;
	for (i=num=0; i<GRID_N; i++)
	{
		for (j=0; j<GRID_N; j++)
		{
			if ( !board[i][j] )
			{
				for (k=i-2; k<=i+2; k++)
				{
					for (l=j-2; l<=j+2; l++)
					{
						if ( k>=0&&k<GRID_N && l>=0&&l<GRID_N && board[k][l] )
						{
							AvaChoices[num].r = i;
							AvaChoices[num].c = j;
							AvaChoices[num++].prior = 
								evalPrior(i, j, AI_CHESS)
							  + evalPrior(i, j, H1_CHESS);
							goto ex0;
						}
					}
				}
			}
			ex0:;
		}
	}

	quickSort( AvaChoices, num);

	for (i=KillNum=0; i<num; i++)
	{
		if (AvaChoices[i].prior>=KILL_PRIOR)
			++KillNum;
		else break;
	}

	if( KillNum>=3 && KillNum+KILL_NUM<num )			  // 出现杀棋时 进行剪枝
	{
#ifdef __DEBUG
		D_cut_hur += num - (KillNum + KILL_NUM);
#endif
		num = KillNum + KILL_NUM ;
	}
	return num;
}


int killSearchChoices( Choice* AvaChoices )
{
	int i, j, k, l, num, temp;
	for (i=num=0; i<GRID_N; i++)
	{
		for (j=0; j<GRID_N; j++)
		{
			if ( !board[i][j] )
			{
				for (k=i-2; k<=i+2; k++)
				{
					for (l=j-2; l<=j+2; l++)
					{
						if ( k>=0&&k<GRID_N && l>=0&&l<GRID_N && board[k][l] )
						{
							temp = evalKillPrior(i, j, AI_CHESS) + evalKillPrior(i, j, H1_CHESS);
							if (temp)
							{
								AvaChoices[num].r = i;
								AvaChoices[num].c = j;
								AvaChoices[num++].prior = temp;
							}
							goto ex1;
						}
					}
				}
			}
			ex1:;
		}
	}

	if(num)
	{
		quickSort( AvaChoices, num);
	}
	return num;
}

// 用于快排的比较函数

void quickSort(Choice *a, int n)
{
	if ( n<=1 )
		return;

	int i, j;
	if ( n>=2 && n<5 )
	{
		for(i=0; i<n-1; i++)
			for(j=i; j<n; j++)
				ifSwapChoice(a[i], a[j]);
		return;
	}

	i=0, j=n-2;
	int mid = n/2;
	ifSwapChoice(a[0], a[mid]);
	ifSwapChoice(a[0], a[n-1]);
	ifSwapChoice(a[mid], a[n-1]);
	swapChoice(a[mid], a[n-2]);
	int pivot = a[n-2].prior;

	while(1)
	{
		while ( a[++i].prior > pivot );
		while ( a[--j].prior < pivot );
		if (i<j)
			swapChoice(a[i], a[j]);
		else break;
	}
	swapChoice(a[i], a[n-2]);
	quickSort(a, i);
	quickSort(a+i+1, n-i-1);
}

void merge(Choice *a, int n_left, int n_right)
{
	int i = 0, j = n_left, cnt = 0;
	int n_sum = n_left+n_right;

	while(i<n_left&&j<n_sum)
	{
		if(a[i].prior>=a[j].prior)
			tp[cnt++] = a[i++];
		else
			tp[cnt++] = a[j++];
	}

	while (i<n_left)
		tp[cnt++] = a[i++];
	while (j<n_sum)
		tp[cnt++] = a[j++];

	memcpy(a, tp, sizeof(Choice)*cnt);
}

void mergeSort(Choice *a, int n)
{
	if(n<=1) return;
	int mid = n/2;	// 左边mid个，右边n-mid个
	mergeSort(a, mid);
	mergeSort(a+mid, n-mid);
	merge(a, mid, n-mid);
}

// 随机生成第一回合的AI落子点
Point RandFirst()
{
	const int CTR = 7;
	Point ret;

	int InitPosFlag = rand()%4;
	switch( InitPosFlag )
	{
		case 0:
			ret.x = CTR-1; ret.y = CTR-1;
			break;
		case 1:
			ret.x = CTR+1; ret.y = CTR-1;
			break;
		case 2:
			ret.x = CTR-1; ret.y = CTR+1;
			break;
		case 3:
			ret.x = CTR+1; ret.y = CTR+1;
			break;
	}
	return ret;
}

// 胜负判断，判断NowPlayer是否胜利 
int Win(int x, int y, Chessid NowPlayer)
{
	int r, c, cnt;

	r=x, c=y, cnt=0;
	while (c<GRID_N && board[r][c] == NowPlayer)
		cnt++, c++;
	c = y-1;
	while ( c>=0 && board[r][c] == NowPlayer)
		cnt++, c--;
	if (cnt>=5)
		return 1;

	r=x, c=y, cnt=0;
	while ( r<GRID_N && board[r][c] == NowPlayer)
		cnt++, r++;
	r = x-1;
	while (  r>=0 && board[r][c] == NowPlayer)
		cnt++, r--;
	if (cnt>=5)
		return 1;

	r=x, c=y, cnt=0;
	while ( c<GRID_N && r<GRID_N && board[r][c] == NowPlayer)
		cnt++,c++,r++;
	c=y-1, r=x-1;
	while ( r>=0  && c>=0  && board[r][c] == NowPlayer)
		cnt++, c--, r--;
	if (cnt>=5)
		return 1;

	r=x, c=y, cnt=0;
	while ( c<GRID_N && r>=0  && board[r][c] == NowPlayer)
		cnt++, c++, r--;
	c=y-1, r=x+1;
	while ( c>=0  && r<GRID_N && board[r][c] == NowPlayer)
		cnt++, c--, r++;
	if (cnt>=5)
		return 1;

	return 0;
}

