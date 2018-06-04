#include <stdlib.h>
#include <time.h>
#include "ai_algorithm.h"
#include "ai_evaluate.h"

const int WinScore = 140000;	// 与 C5Score 保持同步
const int KillScore = 15100;	// 与DTA3Score保持同步
const int Difficulty = 5;		// 设定AI难度


//  利用极大极小搜索算法 遍历博弈树
//  并利用Alpha-Beta剪枝算法优化
int MinMaxSearch(int x, int y, int NowPlayer, int depth, int alpha, int beta)
{
	int score, NumOfAva;
	if ( Win(x, y, NowPlayer) )
	{
		return NowPlayer * WinScore;
	}
	else if ( Win(x, y, -NowPlayer) )
	{
		return -NowPlayer * WinScore;
	}

	if ( depth>=Difficulty )
	{
		return EvalBoard(x, y, NowPlayer);
	}
	else if ( NowPlayer==AI )
	{
		int k, r, c;
		Choice AvaChoices[LEN*LEN];

//  调用启发式搜索函数 按序生成待遍历的子结点 
		NumOfAva = HeuriSearchChoices( AvaChoices );

		for (k=0; k<NumOfAva; k++)   	
		{
			r = AvaChoices[k].r;
			c = AvaChoices[k].c;

			board[r][c] = AI;			// 临时落子 
			score = MinMaxSearch(r, c, PLY, depth+1, alpha, beta); // 递归计算下一层分数 
			board[r][c] = 0;			// 撤子

			if (score > alpha)			// 对于AI，更新极大值 
			{
				if (!depth)
				{
					AIChoice.r = r;
					AIChoice.c = c;
				}
				alpha = score;
			}
			if (alpha >= beta)   			// Alpha-剪枝
			{
				return alpha;
			}
		}
		return alpha;
	}
	else // if ( NowPlayer==PLY )
	{
		int r, c, k;
		Choice AvaChoices[LEN*LEN];

		NumOfAva = HeuriSearchChoices(AvaChoices);

		for (k=0; k<NumOfAva; k++)
		{
			r = AvaChoices[k].r;
			c = AvaChoices[k].c;

			board[r][c] = PLY;			// 临时落子 
			score = MinMaxSearch(r, c, AI, depth+1, alpha, beta); // 递归计算下一层分数 
			board[r][c] = 0;			// 撤子

			if (score < beta)			// 对于玩家，更新极小值
			{
				beta = score ;
			}
			if (alpha >= beta)   			// Beta-剪枝
			{
				return beta ;
			}
		}
		return beta;
	}
}

//  利用启发式搜索优化Alpha-Beta剪枝
int HeuriSearchChoices( Choice* AvaChoices )
{
	int i,j,k,l,cnt,num,KillNum;
	for (i=num=0; i<LEN; i++)
	{
		for (j=0; j<LEN; j++)
		{

			if ( !board[i][j] )
			{
				for (k=i-2, cnt=0; k<=i+2; k++)
				{
					for (l=j-2; l<=j+2; l++)
					{
						if ( board[k][l] )
							cnt ++;
					}
				}

				if (cnt)
				{
					AvaChoices[num].r = i;

					AvaChoices[num].c = j;

					AvaChoices[num].prior = 
						EvalPrior(i, j, AI)
					  + EvalPrior(i, j, PLY);// 根据 ai_evaluate.h 中的 EvalPrior 函数 
								// 对当前待落子点进行优先级评估
					num++;
				}

			}
		}
	}

	qsort( AvaChoices, num,					// 根据优先级评估分数进行排序 
		sizeof(Choice), ChoiceCMP);

	for (i=KillNum=0; i<num; i++)
	{
		if (AvaChoices[i].prior>=KillScore)
		{
			KillNum ++;
		}
		else break;
	}

	if( KillNum && KillNum+5<num )				// 出现杀棋时 进行剪枝
		num = KillNum + 5 ;
	return num;
}

// 用于快排的比较函数 
int ChoiceCMP(const void *a, const void *b)
{
	Choice *p=(Choice *)a, *q=(Choice *)b;
	return p->prior < q->prior? 1:-1;
}

// 随机生成第一回合的AI落子点
Chess RandFirst()
{
	srand((unsigned)time(NULL));

	const int CTR = 7;
	Chess ret;

	int InitPosFlag = rand()%8;
	switch( InitPosFlag )
	{
		case 0:
			ret.r = CTR-1; ret.c = CTR-1;
			break;
		case 1:
			ret.r = CTR-1; ret.c = CTR;
			break;
		case 2:
			ret.r = CTR-1; ret.c = CTR+1;
			break;
		case 3:
			ret.r = CTR;   ret.c = CTR-1;
			break;
		case 4:
			ret.r = CTR;   ret.c = CTR+1;
			break;
		case 5:
			ret.r = CTR+1; ret.c = CTR-1;
			break;
		case 6:
			ret.r = CTR+1; ret.c = CTR;
			break;
		case 7:
			ret.r = CTR+1; ret.c = CTR+1;
			break;
	}
	return ret;
}

// 胜负判断，判断NowPlayer是否获胜（构成连五）
int Win(int x, int y, int NowPlayer)
{
	int r, c, cnt;

	r=x, c=y, cnt=0;
	while (c<LEN && board[r][c] == NowPlayer)
		cnt++, c++;
	c = y-1;
	while ( c>=0 && board[r][c] == NowPlayer)
		cnt++, c--;
	if (cnt>=5)
		return 1;

	r=x, c=y, cnt=0;
	while ( r<LEN && board[r][c] == NowPlayer)
		cnt++, r++;
	r = x-1;
	while (  r>=0 && board[r][c] == NowPlayer)
		cnt++, r--;
	if (cnt>=5)
		return 1;

	r=x, c=y, cnt=0;
	while ( c<LEN && r<LEN && board[r][c] == NowPlayer)
		cnt++,c++,r++;
	c=y-1, r=x-1;
	while ( r>=0  && c>=0  && board[r][c] == NowPlayer)
		cnt++, c--, r--;
	if (cnt>=5)
		return 1;

	r=x, c=y, cnt=0;
	while ( c<LEN && r>=0  && board[r][c] == NowPlayer)
		cnt++, c++, r--;
	c=y-1, r=x+1;
	while ( c>=0  && r<LEN && board[r][c] == NowPlayer)
		cnt++, c--, r++;
	if (cnt>=5)
		return 1;

	return 0;
}
