/**
 * Author: Kevin
 * Date: 2018-06-03
 * Description: 基础声明 
 */

#ifndef _BASE_H
#define _BASE_H

#define LEN 15

enum { AI = 1, PLY = -1, PLY2 = 2 };

extern char __board [LEN+4][LEN+4];
extern char (*const board)[LEN];
extern const char WEIGHT[LEN][LEN];

struct _Point {
    int x;
    int y;
};
typedef struct _Point Point;

struct _Chess {
    int r;
    int c;
};
typedef struct _Chess Chess;

extern Chess AIChoice;

struct _Choice {
    int r;
    int c;
    int prior;
};
typedef struct _Choice Choice;


#endif
