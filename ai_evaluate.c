#include <string.h>
#include "ai_evaluate.h"

// 定义14种棋型分数 
const int C5Score   =  135000;   // 连五
const int A4Score   =  50000;    // 活四
const int P4Score   =  7000;	 // 冲四
const int DP4Score  =  20050;	 // 双冲四
const int P4TA3Score=  18500;	 // 冲四活三
const int TA3Score  =  5100;	 // 真活三
const int DTA3Score =  15000;	 // 双真活三
const int FA3Score  =  449;      // 伪活三
const int TS3Score  =  500;      // 真眠三
const int FS3Score  =  274;	 // 伪眠三
const int TA2Score  =  400;      // 真活二
const int FA2Score  =  99;       // 伪活二
const int TS2Score  =  175;      // 真眠二
const int FS2Score  =  49;       // 伪眠二


// 哈希表 存储当前棋型
typedef struct _HashTable{

	char C5;		// 连五
	char A4;		// 活四 
	char P4;		// 冲四 
	char DP4;		// 双冲四
	char P4TA3;		// 冲四活三
	char TA3;		// 真活三 
	char DTA3;		// 双真活三 
	char FA3;		// 伪活三 
	char TS3;		// 真眠三 
	char FS3;		// 伪眠三 
	char TA2;		// 真活二 
	char FA2;		// 伪活二 
	char TS2;		// 真眠二 
	char FS2;		// 伪眠二 

} HashTable;

HashTable __ChessType[3];
HashTable *const ChessType = __ChessType + 1;	
// 指针形式引用 便于操作
// ChessType[AI] 记录 AI 当前棋型 
// ChessType[PLY]记录玩家当前棋型 



// 分析棋盘纵向15路棋型
void AnalysisVertical()							
{
	int TempLine[LEN];
	int r,c;
	for(c=0; c<LEN; c++)
	{
		for(r=0; r<LEN; r++)
		{
			TempLine[r] = board[r][c];
		}
		ScanLine(TempLine, LEN);
	}
}

// 分析棋盘横向15路棋型
void AnalysisHorizontal()
{
	int TempLine[LEN];
	int r,c;
	for(r=0; r<LEN; r++)
	{
		for(c=0; c<LEN; c++)
		{
			TempLine[c] = board[r][c];
		}
		ScanLine(TempLine, LEN);
	}
}

// 分析棋盘斜向42路棋型
void AnalysisInclined()
{
	int TempLine1[LEN], TempLine2[LEN];
	int x,y,i;

// 自底向上22路
	for(i=LEN-5; i>=0; i--)
	{
		for(x=i,y=0; x<LEN; x++,y++)
		{
			TempLine1[y] = board[x][y];
			TempLine2[y] = board[LEN-1-y][x];
		}
		ScanLine(TempLine1, y);
		ScanLine(TempLine2, y);

// 自顶向下20路
		if(i)
		{
			for(x=0,y=i; y<LEN; x++,y++)
			{
				TempLine1[x] = board[x][y];
				TempLine2[x] = board[LEN-1-y][x];
			}
			ScanLine(TempLine1, x);
			ScanLine(TempLine2, x);
		}
	}
}

// 分析线型棋型（核心函数） 
void ScanLine(const int* const Line, int n)
{
	int i, j, k, cnt, InitPos, FinalPos, NowPlayer;
	int LeftBlank, RightBlank, sumscore = 0;
	int extra, ExtraBlank;

	for (i=0; i<n; i++)
	{
		if ( Line[i] )
		{
//  利用 FinalPos 记录始端位置 
			InitPos = i-1;
			NowPlayer = Line[i];

//  利用 cnt 记录连子个数 
			for (FinalPos=i, cnt=extra=ExtraBlank=0; FinalPos<n&&Line[FinalPos]==NowPlayer; FinalPos++)
			{
				cnt++;
			}

			if ( !Line[FinalPos] && FinalPos+1<n && Line[FinalPos+1]==NowPlayer ){
				for(j=FinalPos+1; j<n&&Line[j]==NowPlayer; j++)
					extra++;
				if(cnt+extra>=3)
				{
					FinalPos = j;
					ExtraBlank = 1;	// 认同跳棋型 
				}
				else
				{
					extra=0;		// 不认同 
				}
			}
//  利用 FinalPos 记录末端位置 
			i = FinalPos-1;

			if (cnt==1&&extra==0)
			{
				continue;
			}
			if (cnt>=5||extra>=5)
			{
				ChessType[NowPlayer].C5 ++;
			}

//  利用 Left/RightBlank 记录空白棋格个数 
			for (k=InitPos, LeftBlank=0; k>=0&&LeftBlank<=2&&!Line[k]; k--)
			{
				LeftBlank++;
			}
			for (k=FinalPos, RightBlank=0; k<n&&RightBlank<=2&&!Line[k]; k++)
			{
				RightBlank++;
			}

//  根据连子个数和空白棋格个数择出棋型 存入棋型哈希表
			if(ExtraBlank==0)
			{
				switch(cnt)
				{
					case 2:
						if ( LeftBlank>=2&&RightBlank || RightBlank>=2&&LeftBlank )
							ChessType[NowPlayer].TA2 ++;
						else if ( LeftBlank && RightBlank )
							ChessType[NowPlayer].FA2 ++;
						else if ( LeftBlank>=2 || RightBlank>=2 )
							ChessType[NowPlayer].TS2 ++;
						else if ( LeftBlank + RightBlank )
							ChessType[NowPlayer].FS2 ++;
						break;
	
					case 3:
						if ( LeftBlank>=2&&RightBlank || RightBlank>=2&&LeftBlank )
							ChessType[NowPlayer].TA3 ++;
						else if ( LeftBlank && RightBlank )
							ChessType[NowPlayer].FA3 ++;
						else if ( LeftBlank>=2 || RightBlank>=2 )
							ChessType[NowPlayer].TS3 ++;
						else if ( LeftBlank + RightBlank )
							ChessType[NowPlayer].FS3 ++;
						break;
	
					case 4:
						if ( LeftBlank && RightBlank )
							ChessType[NowPlayer].A4 ++;
						else if ( LeftBlank + RightBlank )
							ChessType[NowPlayer].P4 ++;
						break;
				}
			}
			else
			{
				if ( cnt==4&&LeftBlank || extra==4&&RightBlank )
					ChessType[NowPlayer].A4 ++;
				else if ( cnt+extra >= 4 )
					ChessType[NowPlayer].P4 ++;
				else // if (cnt+extra == 3)
				{
					if ( LeftBlank&&RightBlank )
						ChessType[NowPlayer].TA3 ++;
					else if ( LeftBlank||RightBlank )
						ChessType[NowPlayer].TS3 ++;
				}
			}
		}
	}
}

//  对当前棋局分数进行评估
int EvalBoard(int x, int y, int NowPlayer)
{
	int SumScore = NowPlayer * WEIGHT[x][y];
	memset (__ChessType, 0, sizeof(__ChessType));

	AnalysisVertical();

	AnalysisHorizontal();

	AnalysisInclined();

//  补充判断双冲四、双真活三、冲四活三棋型
	if (ChessType[AI].P4 >=2)
	{
		ChessType[AI].DP4 ++;
		ChessType[AI].P4 -=2;
	}

	if (ChessType[AI].TA3 >= 2)
	{
		ChessType[AI].DTA3 ++;
		ChessType[AI].TA3 -=2;
	}

	if (ChessType[AI].P4 && ChessType[AI].TA3)
	{
		ChessType[AI].P4TA3 ++;
		ChessType[AI].P4 --;
		ChessType[AI].TA3 --;
	}
	
	if (ChessType[PLY].P4 >=2)
	{
		ChessType[PLY].DP4 ++;
		ChessType[PLY].P4 -=2;
	}

	if (ChessType[PLY].TA3 >= 2)
	{
		ChessType[PLY].DTA3 ++;
		ChessType[PLY].TA3 -=2;
	}

	if (ChessType[PLY].P4 && ChessType[PLY].TA3)
	{
		ChessType[PLY].P4TA3 ++;
		ChessType[PLY].P4 --;
		ChessType[PLY].TA3 --;
	}


//  计算当前棋局总分
	SumScore  += ChessType[AI].C5	 *  C5Score
	          +  ChessType[AI].A4	 *  A4Score
	          +  ChessType[AI].P4	 *  P4Score
	          +  ChessType[AI].DP4   *  DP4Score
	          +  ChessType[AI].P4TA3 *  P4TA3Score
	          +  ChessType[AI].TA3   *  TA3Score
	          +  ChessType[AI].DTA3  *  DTA3Score
	          +  ChessType[AI].FA3   *  FA3Score
	          +  ChessType[AI].TS3	 *  TS3Score
	          +  ChessType[AI].FS3	 *  FS3Score
	          +  ChessType[AI].TA2	 *  TA2Score
	          +  ChessType[AI].FA2	 *  FA2Score
	          +  ChessType[AI].TS2	 *  TS2Score
	          +  ChessType[AI].FS2	 *  FS2Score

	          -  ChessType[PLY].C5   *  C5Score
	          -  ChessType[PLY].A4   *  A4Score
	          -  ChessType[PLY].P4   *  P4Score
	          -  ChessType[PLY].DP4  *  DP4Score
	          -  ChessType[PLY].P4TA3*  P4TA3Score
	          -  ChessType[PLY].TA3  *  TA3Score
	          -  ChessType[PLY].DTA3 *  DTA3Score
	          -  ChessType[PLY].FA3  *  FA3Score
	          -  ChessType[PLY].TS3  *  TS3Score
	          -  ChessType[PLY].FS3  *  FS3Score
	          -  ChessType[PLY].TA2  *  TA2Score
	          -  ChessType[PLY].FA2  *  FA2Score
	          -  ChessType[PLY].TS2  *  TS2Score
	          -  ChessType[PLY].FS2  *  FS2Score ;

	return SumScore;
}

//  对某一落子点进行优先级值评估，用于启发式搜索中的排序参考 
int EvalPrior(int x, int y, int NowPlayer)
{
	int PriorScore = WEIGHT[x][y];

//  NowType存储当前棋型 
	HashTable NowType = {0}; 

//  记录连子末端延伸方向的空格数
	int up, dw, lf, rt, lu, ld, ru, rd;
//  记录四向连子个数
	int r, c, cnth, cntv, cntru, cntrd ;

	cnth = cntv = cntru = cntrd = 1;
	lf = rt = up = dw = lu = ld = ru = rd = 0;

//  保存当前位置状态并临时落子 
	int NowState = board[x][y];
	board[x][y] = NowPlayer;

//  横向快速扫描棋型
	r=x, c=y+1;
	while (c<LEN && board[r][c]==NowPlayer)
		cnth++, c++;
	while (c<LEN && !board[r][c] && rt<=2)
		rt++, c++;

	r=x, c=y-1;
	while (c>=0 && board[r][c]==NowPlayer)
		cnth++, c--;
	while (c>=0 && !board[r][c] && lf<=2)
		lf++, c--;

//  纵向快速扫描棋型
	r=x+1,c=y;
	while (r<LEN && board[r][c]==NowPlayer)
		cntv++,r++;
	while (r<LEN && !board[r][c] && dw<=2)
		dw++,r++;

	r=x-1,c=y;
	while (r>=0 && board[r][c]==NowPlayer)
		cntv++,r--;
	while (r>=0 && !board[r][c] && up<=2)
		up++,r--;

//  斜向上快速扫描棋型 
	r=x-1,c=y+1;
	while (r>=0 && c<LEN && board[r][c]==NowPlayer)
		cntru++,r--,c++;
	while (r>=0 && c<LEN && !board[r][c] && ru<=2)
		ru++,r--,c++;

	r=x+1, c=y-1;
	while (r<LEN && c>=0 && board[r][c]==NowPlayer)
		cntru++,r++,c--;
	while (r<LEN && c>=0 && !board[r][c] && ld<=2)
		ld++,r++,c--;

//  斜向下快速扫描棋型 
	r=x+1, c=y+1;
	while (r<LEN && c<LEN && board[r][c]==NowPlayer)
		cntrd++,r++,c++;
	while (r<LEN && c<LEN && !board[r][c] && rd<=2)
		rd++,r++,c++;

	r=x-1, c=y-1;
	while (r>=0 && c>=0 && board[r][c]==NowPlayer)
		cntrd++,r--,c--;
	while (r>=0 && c>=0 && !board[r][c] && lu<=2)
		lu++,r--,c--;

//  撤子，回归初始位置状态	
	board[x][y] = NowState;

//  对单子影响力进行评估： 

	// 连五
	if	(cnth >= 5 || cntv >= 5 || cntru >= 5 || cntrd >= 5)
		return C5Score;

	// 活四
	if	(	( cnth == 4 && lf && rt )||( cntv == 4 && up && dw ) ||
	        ( cntru== 4 && ld && ru )||( cntrd== 4 && lu && rd )
	    )
		return A4Score;
	// 冲四
	else {
		NowType.P4 = ( cnth == 4 && lf+rt )+( cntv == 4 && up+dw ) +
	        	     ( cntru== 4 && ld+ru )+( cntrd== 4 && lu+rd ) ;
	    if (NowType.P4>=2)
	    	return DP4Score;
	}
	// 双真活三
 	NowType.TA3 = (cnth == 3 && ( lf>=2&&rt || rt>=2&&lf )) +
			      (cntv == 3 && ( up>=2&&dw || dw>=2&&up )) +
			      (cntru== 3 && ( ld>=2&&ru || ru>=2&&ld )) +
			      (cntrd== 3 && ( lu>=2&&rd || rd>=2&&lu )) ;
	if (NowType.TA3>=2)
		return DTA3Score;
	// 冲四活三 
	else if (NowType.TA3&&NowType.P4)
		return P4TA3Score;
	// 真活三
	else if (NowType.TA3)
		return TA3Score;
		
	else if(( cnth == 3 && lf && rt ) || ( cntv == 3 && up && dw ) ||
	        ( cntru== 3 && ld && ru ) || ( cntrd== 3 && lu && rd )
	    )
		return PriorScore + FA3Score;
	// 真眠三
	else if(( cnth == 3 && (lf>=2||rt>=2) ) || ( cntv == 3 && (up>=2||dw>=2) ) ||
	        ( cntru== 3 && (ld>=2||ru>=2) ) || ( cntrd== 3 && (lu>=2||rd>=2) )
	    )
		return PriorScore + TS3Score;
	// 伪眠三
	else if(( cnth == 3 && lf+rt ) || ( cntv == 3 && up+dw ) ||						
	        ( cntru== 3 && ld+ru ) || ( cntrd== 3 && lu+rd )
	    )
		PriorScore += FS3Score;
	// 双真活二
	NowType.TA2 = (cnth == 2 && ( lf>=2&&rt || rt>=2&&lf )) +
			      (cntv == 2 && ( up>=2&&dw || dw>=2&&up )) +
			      (cntru== 2 && ( ld>=2&&ru || ru>=2&&ld )) +
			      (cntrd== 2 && ( lu>=2&&rd || rd>=2&&lu )) ;
	if  ( NowType.TA2>=2 )
		return PriorScore += 1.2*TA2Score;
	// 真活二
	else if( NowType.TA2 )
		return PriorScore += TA2Score;
	// 伪活二
	else if(( cnth == 2 && lf && rt ) || ( cntv == 2 && up && dw ) ||
	        ( cntru== 2 && ld && ru ) || ( cntrd== 2 && lu && rd )
	    )
		return PriorScore += FA2Score;
	// 真眠二
	else if(( cnth == 2 && (lf>=2||rt>=2) ) || ( cntv == 2 && (up>=2||dw>=2) ) ||
	        ( cntru== 2 && (ld>=2||ru>=2) ) || ( cntrd== 2 && (lu>=2||rd>=2) )
	    )
		return PriorScore += TS2Score;
	// 伪眠二
	else if(( cnth == 2 && lf+rt ) || ( cntv == 2 && up+dw ) ||						
	        ( cntru== 2 && ld+ru ) || ( cntrd== 2 && lu+rd )
	    )
		PriorScore += FS2Score;

	return PriorScore;
}
