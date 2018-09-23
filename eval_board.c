#include "eval_board.h"
#include <string.h>
#include <stdlib.h>
#include <time.h>

#define M_0		0		// 空棋型
#define M_AI_C5		0x1		// AI  连五
#define M_HU_C5		0x2		// Man 连五
#define M_AI_A4		0x4		// AI  活四
#define M_HU_A4		0x8		// Man 活四
#define M_AI_P4		0x10		// AI  冲四
#define M_HU_P4		0x20		// Man 冲四
#define M_AI_A3		0x40		// AI  真活三
#define M_HU_A3		0x80		// Man 真活三
#define M_AI_S3		0x100		// AI  真眠三 或 伪活三
#define M_HU_S3		0x200		// Man 真眠三 或 伪活三
#define M_AI_A2		0x400		// AI  真活二
#define M_HU_A2		0x800		// Man 真活二
#define M_AI_S2		0x1000		// AI  真眠二 或 伪活二
#define M_HU_S2		0x2000		// Man 真眠二 或 伪活二
#define M_AI_A1		0x4000		// AI  真活一
#define M_HU_A1		0x8000		// Man 真活一

#define M_C5		0x3			// 连五
#define M_A4		0xc			// 活四
#define M_P4		0x30		// 冲四
#define M_A3		0xc0		// 真活三
#define M_S3		0x300		// 真眠三 或 伪活三
#define M_A2		0xc00		// 真活二

#define M_DAI_P4		0xffef
#define M_DHU_P4		0xffdf
#define M_DAI_A3		0xffbf
#define M_DHU_A3		0xff7f
#define M_DAI_S3		0xfeff
#define M_DHU_S3		0xfdff

#define I_0			0			// 空棋型
#define I_AI_C5		1			// AI  连五
#define I_HU_C5		2			// Man 连五
#define I_AI_A4		3			// AI  活四
#define I_HU_A4		4			// Man 活四
#define I_AI_P4		5			// AI  冲四
#define I_HU_P4		6			// Man 冲四
#define I_AI_A3		7			// AI  真活三
#define I_HU_A3		8			// Man 真活三
#define I_AI_S3		9			// AI  真眠三 或 伪活三
#define I_HU_S3		10			// Man 真眠三 或 伪活三
#define I_AI_A2		11			// AI  真活二
#define I_HU_A2		12			// Man 真活二
#define I_AI_S2		13			// AI  真眠二 或 伪活二
#define I_HU_S2		14			// Man 真眠二 或 伪活二
#define I_AI_A1		15			// AI  真活一
#define I_HU_A1		16			// Man 真活一

#define C5score     135000	 // 连五
#define A4score     25000	 // 活四
#define P4score     5200	 // 冲四
#define A3score		4990	 // 活三
#define S3score		410		 // 眠三
#define A2score		450		 // 活二
#define S2score		70		 // 眠二
#define A1score		30		 // 活一

#define Plus44score	10000	 // 奖励
#define Plus34score	6550	 // 奖励

#define BWN			17			// 棋型种数

#define MASK 0xfff
#define WIN_SCORE_FIX C5score +10-depth
#define LOSE_SCORE_FIX -C5score -10+depth
#define RUSH(x) for(x=0; x<3; ++x)
#define mem0(x) memset(x, 0, sizeof(x))
#define rand64() rand() ^ ((__int64)rand()<<15) ^ ((__int64)rand()<<30) ^ ((__int64)rand()<<45) ^ ((__int64)rand()<<60)

#define _ini(x, QX) M_TABLE[x] = M_##QX

#define _reset_(QX, line_i_type)	if(line_i_type & M_AI_##QX) --all_type[I_AI_##QX]; \
									if(line_i_type & M_HU_##QX) --all_type[I_HU_##QX]

#define _set_(QX, line_i_type)	if(line_i_type & M_AI_##QX) ++all_type[I_AI_##QX]; \
								if(line_i_type & M_HU_##QX) ++all_type[I_HU_##QX]

#define _plus_(QX, line_i_type, sb, sw)	if(line_i_type & M_AI_##QX) sb += QX##score;	\
										if(line_i_type & M_HU_##QX) sw += QX##score

#define _eval_(QX, sb, sw)		sb += all_type[I_AI_##QX] * QX##score; \
								sw += all_type[I_HU_##QX] * QX##score

#define RESET(line, x, line_i_type)			\
			_reset_(C5, line_i_type);		\
			_reset_(A4, line_i_type);		\
			_reset_(P4, line_i_type);		\
			_reset_(A3, line_i_type);		\
			_reset_(S3, line_i_type);		\
			_reset_(A2, line_i_type);		\
			_reset_(S2, line_i_type);		\
			_reset_(A1, line_i_type);		\
			line##_type[x] = 0

#define SET(line_i_type)				\
			_set_(C5, line_i_type);		\
			_set_(A4, line_i_type);		\
			_set_(P4, line_i_type);		\
			_set_(A3, line_i_type);		\
			_set_(S3, line_i_type);		\
			_set_(A2, line_i_type);		\
			_set_(S2, line_i_type);		\
			_set_(A1, line_i_type)

#define EVAL(sb, sw)				\
			_eval_(C5, sb, sw);		\
			_eval_(A4, sb, sw);		\
			_eval_(P4, sb, sw);		\
			_eval_(A3, sb, sw);		\
			_eval_(S3, sb, sw);		\
			_eval_(A2, sb, sw);		\
			_eval_(S2, sb, sw);		\
			_eval_(A1, sb, sw)

#define PLUS(line_i_type, sb, sw)					\
			_plus_(C5, line_i_type, sb, sw);		\
			_plus_(A4, line_i_type, sb, sw);		\
			_plus_(P4, line_i_type, sb, sw);		\
			_plus_(A3, line_i_type, sb, sw);		\
			_plus_(S3, line_i_type, sb, sw);		\
			_plus_(A2, line_i_type, sb, sw);		\
			_plus_(S2, line_i_type, sb, sw);		\
			_plus_(A1, line_i_type, sb, sw)

#define __dl(line, x, BW)					\
	if (line##_type[x] & M_##BW##A4)		\
	{										\
		line##_type[x] &= M_D##BW##P4;		\
		if (line##_type[x] & M_##BW##S3)	\
			line##_type[x] &= M_D##BW##S3;	\
	}										\
	else if (line##_type[x] & M_##BW##P4)	\
	{										\
		if (line##_type[x] & M_##BW##A3)	\
			line##_type[x] &= M_D##BW##A3;	\
		if (line##_type[x] & M_##BW##S3)	\
			line##_type[x] &= M_D##BW##S3;	\
	}

#define DEAL_34(line, x)		\
	do {						\
		__dl(line, x, AI_)		\
		__dl(line, x, HU_)		\
	} while(0)

#define EVAL_PRIOR(line)						\
		if(M_TABLE[now_##line & MASK] & M_C5)	\
			return 16384;						\
		if(M_TABLE[now_##line & MASK] & M_A4)	\
		{										\
			sum_score += 1024;					\
			break;								\
		}										\
		if(M_TABLE[now_##line & MASK] & M_P4)	\
		{										\
			sum_score += 256;					\
			break;								\
		}										\
		if(M_TABLE[now_##line & MASK] & M_A3)	\
		{										\
			sum_score += 256;					\
			break;								\
		}										\
		if(M_TABLE[now_##line & MASK] & M_A2)	\
		{										\
			sum_score += 40;					\
			break;								\
		}										\
		if(M_TABLE[now_##line & MASK] & M_S3)	\
		{										\
			sum_score += 30;					\
			break;								\
		}


#define EVAL_KILL_PRIOR(line)					\
		if(M_TABLE[now_##line & MASK] & M_C5)	\
			return 100;							\
		if(M_TABLE[now_##line & MASK] & M_A4)	\
		{										\
			kill_num += 16;						\
			break;								\
		}										\
		if(M_TABLE[now_##line & MASK] & M_P4)	\
		{										\
			++kill_num;							\
			break;								\
		}										\
		if(M_TABLE[now_##line & MASK] & M_A3)	\
		{										\
			++kill_num;							\
			break;								\
		}


__int32 col_chess[GRID_N];
__int32 row_chess[GRID_N];
__int32 left_chess[GRID_DN];
__int32 right_chess[GRID_DN];

__int32 col_type[GRID_N];
__int32 row_type[GRID_N];
__int32 left_type[GRID_DN];
__int32 right_type[GRID_DN];

int all_type[CHESS_TYPE_N];


/*	真眠X 和 伪活X 等价的原因是再加同样的子数就可达到冲四
 *	伪眠三、伪眠二、伪活一、眠一以及所有死子没有价值（无法连五）
 */


/*	规定：
 *	  00B ：空
 * 	  01B ：AI
 * 	  10B ：Man
 */


/*	先用 FFFH 扫过取出某12个bit，再查表即可确定棋型
 *	比如 1111 1111 1111B
 *	   & 0001 0101 0100B
 *     = 0001 0101 0100B
 *
 *	下面打表：
 */

struct _REFER{ int x; const char *a[5]; }
REFERENCE[16] =
{
	{ 0x0, "0000"},		{ 0x4, "0100"},		{ 0x8, "1000"},		{ 0xc, "1100"},
	{ 0x1, "0001"},		{ 0x5, "0101"},		{ 0x9, "1001"},		{ 0xd, "1101"},
	{ 0x2, "0010"},		{ 0x6, "0110"},		{ 0xa, "1010"},		{ 0xe, "1110"},
	{ 0x3, "0011"},		{ 0x7, "0111"},		{ 0xb, "1011"},		{ 0xf, "1111"}
};

// 棋型映射表
int M_TABLE[ 0xaab ];
// 最大数对应的是6颗Man子，也就是 101010101010B，也就是 AAAH
// 所以映射表下标最大是 0xaab

//__int64 zobrist[3][GRID_N][GRID_N], xor_sum, xor_sum0;
// 置换表，约3000w，是2的25次方，取模就是 x & 0x1ffffff 
//int REP_TABLE[ REP_MOD+1 ];

void initBitBoard()
{
	mem0(col_chess);
	mem0(col_type);
	mem0(row_chess);
	mem0(row_type);
	mem0(left_chess);
	mem0(left_type);
	mem0(right_chess);
	mem0(right_type);

	mem0(all_type);
//	xor_sum = xor_sum0;
}

void initHASH()
{
//	srand(time(NULL));
	mem0(M_TABLE);
//	mem0(REP_TABLE);

//	xor_sum0 = rand64();
	
//	for(int i=0; i<GRID_N; ++i)
//		for(int j=0; j<GRID_N; ++j)
//			zobrist[AI][i][j] = rand64();
//
//	for(int i=0; i<GRID_N; ++i)
//		for(int j=0; j<GRID_N; ++j)
//			zobrist[PLY][i][j] = rand64();

	// AI连五
	_ini(0x555, AI_C5);
	_ini(0x554, AI_C5);
	_ini(0x155, AI_C5);
	_ini(0x556, AI_C5);
	_ini(0x955, AI_C5);
	
	// Man连五
	_ini(0xaaa, HU_C5);
	_ini(0xaa8, HU_C5);
	_ini(0x2aa, HU_C5);
	_ini(0xaa9, HU_C5);
	_ini(0x6aa, HU_C5);

	//AI活四
	_ini(0x154, AI_A4);

	//Man活四
	_ini(0x2a8, HU_A4);

	//AI活三
	_ini(0x150, AI_A3);
	_ini(0x144, AI_A3);
	_ini(0x114, AI_A3);
	_ini(0x054, AI_A3);

	//Man活三
	_ini(0x2a0, HU_A3);
	_ini(0x288, HU_A3);
	_ini(0x228, HU_A3);
	_ini(0x0a8, HU_A3);

	//AI活二
	_ini(0x140, AI_A2);
	_ini(0x110, AI_A2);
	_ini(0x104, AI_A2);
	_ini(0x050, AI_A2);
	_ini(0x044, AI_A2);
	_ini(0x014, AI_A2);

	//Man活二
	_ini(0x280, HU_A2);
	_ini(0x220, HU_A2);
	_ini(0x208, HU_A2);
	_ini(0x0a0, HU_A2);
	_ini(0x088, HU_A2);
	_ini(0x028, HU_A2);

	//AI活一
	_ini(0x100, AI_A1);
	_ini(0x040, AI_A1);
	_ini(0x010, AI_A1);
	_ini(0x004, AI_A1);

	//Man活一
	_ini(0x200, HU_A1);
	_ini(0x080, HU_A1);
	_ini(0x020, HU_A1);
	_ini(0x008, HU_A1);

	int x, y, ix, iy;
	int i, i1, i2, i3, i4, i5, i6;

	RUSH (i1) RUSH (i2) RUSH (i3)
	RUSH (i4) RUSH (i5) RUSH (i6)
	{
		x = y = ix = iy = 0;
		if( i1 == 1 ) x++;
		else if( i1 == 2 ) y++;
		if( i2 == 1 ) x++,ix++;
		else if( i2 == 2 ) y++,iy++;
		if( i3 == 1 ) x++,ix++;
		else if( i3 == 2 ) y++,iy++;
		if( i4 == 1 ) x++,ix++;
		else if( i4 == 2 ) y++,iy++;
		if( i5 == 1 ) x++,ix++;
		else if( i5 == 2 ) y++,iy++;
		if( i6 == 1 ) ix++;
		else if( i6 == 2 ) iy++;

		i = i1 + (i2<<2) + (i3<<4) + (i4<<6) + (i5<<8) + (i6<<10);

		if( !M_TABLE[i] )
		{
			//AI冲四
			if( x==4 && y==0 || ix==4 && iy==0 )
				_ini(i, AI_P4);

			//Man冲四
			else if( x==0 && y==4 || ix==0 && iy==4 )
				_ini(i, HU_P4);

			//AI眠三
			else if( x==3 && y==0 || ix ==3 && iy==0 )
				_ini(i, AI_S3);

			//Man眠三
			else if( x==0 && y==3 || ix==0 && iy==3 )
				_ini(i, HU_S3);

			//AI眠二
			else if( x==2 && y==0 || ix==2 && iy==0 )
				_ini(i, AI_S2);

			//Man眠二
			else if( x==0 && y==2 || ix==0 && iy==2 )
				_ini(i, HU_S2);
		}
	}
}

void updateChessType(const int x, const int y)
{
	int now_type, count, i, index;
	__int32 mask = MASK;
	__int32 temp1 = col_type[x];
	__int32 temp2 = row_type[y];

	{
		// 删除之前的行列棋型
		RESET(col, x, temp1);
		RESET(row, y, temp2);
		
		// 重新判断行列棋型
		for(i=0, temp1=col_chess[x], temp2=row_chess[y]; i<10; ++i, temp1>>=2, temp2>>=2)
		{
			col_type[x] |= M_TABLE[temp1 & MASK];
			row_type[y] |= M_TABLE[temp2 & MASK];
		}

		// 处理 4 子冲突 
		DEAL_34(col, x);
		DEAL_34(row, y);
		
		// 记录当前行列棋型 
		temp1 = col_type[x];
		SET(temp1);
		temp2 = row_type[y];
		SET(temp2);
	}

//	 // 有活四，一定也有冲四，可能有眠三，这些都删掉 
//	if (col_type[y] & AI_A4)
//	{
//		col_type[y] &= M_DAI_P4;
//		--col_all[I_AI_P4];
//		if (col_type[y] & AI_S3)
//		{
//			col_type[y] &= M_DAI_S3;
//			--col_all[I_AI_S3];
//		}
//	}
//	
//	// 有冲四，可能有活三、眠三，删掉 
//	else if (col_type[y] & AI_P4)
//	{
//		if (col_type[y] & AI_A3)
//		{
//			col_type[y] &= M_DAI_A3;
//			--col_all[I_AI_A3];
//		}
//		if (col_type[y] & AI_S3)
//		{
//			col_type[y] &= M_DAI_S3;
//			--col_all[I_AI_S3];
//		}
//	}
	

	int l = x - y + 9;
	if (l>=0 && l<GRID_DN)
	{
		temp1 = left_type[l];
		RESET(left, l, temp1);
		
		if (l<10)
			count = l+1;
		else
			count = GRID_DN - l;
		for(i=0, temp1=left_chess[l]; i<count; ++i, temp1>>=2)
			left_type[l] |= M_TABLE[temp1 & MASK];
		
		DEAL_34(left, l);
		
		temp1 = left_type[l];
		SET(temp1);
	}

	int r = x + y - 5;
	if (r>=0 && r<GRID_DN)
	{
		temp2 = right_type[r];
		RESET(right, r, temp2);
		
		if (r<10)
			count = r+1;
		else
			count = GRID_DN - r;
		for(i=0, temp2=right_chess[r]; i<count; ++i, temp2>>=2)
			right_type[r] |= M_TABLE[temp2 & MASK];
		
		DEAL_34(right, r);
		
		temp2 = right_type[r];
		SET(temp2);
	}
}

void takeChess(const int x, const int y)
{
//	xor_sum ^= zobrist[id][y][x];
	board[y][x] = 0;

	col_chess[x] &= ~(3 << 2*y);
	row_chess[y] &= ~(3 << 2*x);

	int l = x - y + 9;
	if (l>=0 && l<GRID_DN)
		if (l<10)
			left_chess[l] &= ~(3 << 2*x);
		else
			left_chess[l] &= ~(3 << 2*y);

	int r = x + y - 5;
	if (r>=0 && r<GRID_DN)
		if (r<10)
			right_chess[r] &= ~(3 << 2*y);
		else
			right_chess[r] &= ~(3 << 28-2*x);
}

void putChess(const int x, const int y, const Chessid id)
{
//	xor_sum ^= zobrist[id][y][x];
	board[y][x] = id;
	col_chess[x] |= id << 2*y;
	row_chess[y] |= id << 2*x;

	int l = x - y + 9;
	if (l>=0 && l<GRID_DN)
		if (l<10)
			left_chess[l] |= id << 2*x;
		else
			left_chess[l] |= id << 2*y;

	int r = x + y - 5;
	if (r>=0 && r<GRID_DN)
		if (r<10)
			right_chess[r] |= id << 2*y;
		else
			right_chess[r] |= id << 28-2*x;
}

int evalBoard()	// 进一步优化：也向evalBoard那样用个位数 
{
//	int try_rep;
//	if(try_rep = REP_TABLE[REP_MOD & xor_sum])
//	{
//#ifdef __DEBUG
//		D_rep_cnt++;
//#endif
//		return try_rep;
//	}

#ifdef __DEBUG
	D_eval_cnt++;
#endif

	int ai_sum = 0, hu_sum = 0;
	EVAL(ai_sum, hu_sum);

	if (all_type[I_AI_P4]>=2)
		ai_sum += Plus44score;
	else if (all_type[I_AI_P4]+all_type[I_AI_A3] >= 2)
		ai_sum += Plus34score;

	if (all_type[I_HU_P4]>=2)
		hu_sum += Plus44score;
	else if (all_type[I_HU_P4]+all_type[I_HU_A3] >= 2)
		hu_sum += Plus34score;

	return ai_sum - hu_sum;
//	return REP_TABLE[REP_MOD & xor_sum] = ai_sum - hu_sum;
}

int evalPrior(const int r, const int c, const Chessid id)	// 大问题，理论上讲这里应该只分析受此点影响的局部，而不是所在的整行整列和整斜行 
{
	int x = c, y = r;
	int sum_score = 0;
	__int32 now_col = col_chess[x] | (id << 2*y),
			now_row = row_chess[y] | (id << 2*x),
			now_left = 0, now_right = 0;

	int i, count;
	for(i=0; i<10; ++i, now_col>>=2)
	{
		EVAL_PRIOR(col)
	}
	
	for(i=0; i<10; ++i, now_row>>=2)
	{
		EVAL_PRIOR(row)
	}

	int l = x - y + 9;
	if (l>=0 && l<GRID_DN)
	{
		if (l<10) now_left = left_chess[l] | (id << 2*x), count = l+1;
		else now_left = left_chess[l] | (id << 2*y), count = GRID_DN-l;
		for(i=0; i<count; ++i, now_left>>=2)
		{
			EVAL_PRIOR(left)
		}
	}

	int R = x + y - 5;
	if (R>=0 && R<GRID_DN)
	{
		if (R<10) now_right = right_chess[R] | (id << 2*y), count = R+1;
		else now_right = right_chess[R] | (id << 28-2*x), count = GRID_DN-R;
		for(i=0; i<count; ++i, now_right>>=2)
		{
			EVAL_PRIOR(right)
		}
	}
	return sum_score;
}

int evalKillPrior(const int r, const int c, const Chessid id)	// 大问题，理论上讲这里应该只分析受此点影响的局部，而不是所在的整行整列和整斜行 
{
	int x = c, y = r;
	int kill_num = 0;
	__int32 now_col = col_chess[x] | (id << 2*y),
			now_row = row_chess[y] | (id << 2*x),
			now_left = 0, now_right = 0;

	int i, count;
	for(i=0; i<10; ++i, now_col>>=2)
	{
		EVAL_KILL_PRIOR(col)
	}

	for(i=0; i<10; ++i, now_row>>=2)
	{
		EVAL_KILL_PRIOR(row)
	}

	int l = x - y + 9;
	if (l>=0 && l<GRID_DN)
	{
		if (l<10) now_left = left_chess[l] | (id << 2*x), count = l+1;
		else now_left = left_chess[l] | (id << 2*y), count = GRID_DN-l;
		for(i=0; i<count; ++i, now_left>>=2)
		{
			EVAL_KILL_PRIOR(left)
		}
	}

	int R = x + y - 5;
	if (R>=0 && R<GRID_DN)
	{
		if (R<10) now_right = right_chess[R] | (id << 2*y), count = R+1;
		else now_right = right_chess[R] | (id << 28-2*x), count = GRID_DN-R;
		for(i=0; i<count; ++i, now_right>>=2)
		{
			EVAL_KILL_PRIOR(right)
		}
	}
	return kill_num;
}

int killed(Chessid nowplayer, int depth)
{
#ifdef __DEBUG
	D_eval_cnt++;
#endif

	if (nowplayer==AI_CHESS)
	{
//	①双方可有任何棋型
		if (all_type[I_AI_C5])
			return WIN_SCORE_FIX+5;			// 我方早就有5了，必胜 
		if (all_type[I_HU_C5])
			return LOSE_SCORE_FIX-5;		// 对方有5，我方必败 
		if (all_type[I_AI_A4] || all_type[I_AI_P4])
			return WIN_SCORE_FIX+4;			// 我方有四必胜
		if (all_type[I_HU_A4] || all_type[I_HU_P4]>=2)
			return LOSE_SCORE_FIX-4;		// 对方有活4或双冲四，我方必败 

//	②双方没有5、对方可能有冲四、我方没有四
		if (all_type[I_HU_P4])
		{
			if (all_type[I_HU_A3] && !all_type[I_AI_A3] && !all_type[I_AI_S3])
				return LOSE_SCORE_FIX-3;	// 对方冲四活三，我方没有3，我方必败 
		}

//	③双方没有5、4
		else
		{
			if (all_type[I_AI_A3])
				return WIN_SCORE_FIX+3;		// 对方没有4，我方有活三，我方必胜
			else if (all_type[I_HU_A3]>=2 && !all_type[I_AI_A3] && !all_type[I_AI_S3])
				return LOSE_SCORE_FIX-2;	// 对方有双活三，我方无活三，我方必败
		}
	}
	else
	{
		if (all_type[I_HU_C5])
			return LOSE_SCORE_FIX-5;// 我方早就有5了，必胜 
		if (all_type[I_AI_C5])
			return WIN_SCORE_FIX+5;// 对方有5，我方必败 
		if (all_type[I_HU_A4] || all_type[I_HU_P4])
			return LOSE_SCORE_FIX-4;// 我方有四必胜
		if (all_type[I_AI_A4] || all_type[I_AI_P4]>=2)
			return WIN_SCORE_FIX+4;// 对方有活4或双冲四，我方必败 

//	②双方没有5、对方可能有冲四、我方没有四
		if (all_type[I_AI_P4])
		{
			if (all_type[I_AI_A3] && !all_type[I_HU_A3] && !all_type[I_HU_S3])
				return WIN_SCORE_FIX+3;// 对方冲四活三，我方没有3，我方必败 
		}

//	③双方没有5、4
		else
		{
			if (all_type[I_HU_A3])
				return LOSE_SCORE_FIX-3;	// 对方没有4，我方有活三，我方必胜
			else if (all_type[I_AI_A3]>=2 && !all_type[I_HU_A3] && !all_type[I_HU_S3])
				return WIN_SCORE_FIX+2;		// 对方有双活三，我方无活三，我方必败 
		}
	}
	return 0;
}
