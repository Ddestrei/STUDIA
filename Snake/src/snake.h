#ifndef __SNAKE_H__
#define __SNAKE_H__

#include "winsys.h"
#include <vector>

class CSnake:public CFramedWindow {
public:
    vector<CPoint> snake;
    int snake_size;
    CPoint apple;
    int width;
    int height;
    // 0 - game
    // 1 - start
    // 2 - information
    // 3 - pause
    // 4 - end game
    int screen;
	
	CPoint move;
	
    CSnake(CPoint p1, CPoint p2, char _c = ' ');

    void paint();

    bool handleEvent(int c);

    void make_apple();

    void move_snake(int x, int y);

    bool check_move(int x, int y);

    void reset_snake();
};

#endif
