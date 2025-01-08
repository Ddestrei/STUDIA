#include "snake.h"
#include "screen.h"
#include <iostream>
#include <cstdlib>
#include <unistd.h>


CSnake::CSnake(CPoint p1, CPoint p2, char _c /*=' '*/):
  CFramedWindow(CRect(p1,p2), _c)
{
    this->screen = 1;
	this->width = p2.x;
	this->height = p2.y;
	this->snake.push_back(CPoint(1,1));
	this->snake_size=1;
	this->make_apple();
	this->move.x=0;
	this->move.y=1;
	
}

void CSnake::paint(){
	CFramedWindow::paint();
    if(this->screen==0) {
        for (int i = 0; i < this->snake_size; i++) {
            gotoyx(this->snake.at(i).y+this->geom.topleft.y, this->snake.at(i).x+this->geom.topleft.x);
            printl("S");
        }
        gotoyx(this->apple.y+this->geom.topleft.y, this->apple.x+this->geom.topleft.x);
        printl("A");
    }
    else if(this->screen==1){
        gotoyx(this->geom.topleft.y+1,this->geom.topleft.x+1);
        printl(" s - to start game");
        gotoyx(this->geom.topleft.y+2,this->geom.topleft.x+1);
        printl(" i - information about game");
    }
    else if(this->screen==2){
        gotoyx(this->geom.topleft.y+1,this->geom.topleft.x+1);
        printl(" This game was developed by Patryk Grys ");
        gotoyx(this->geom.topleft.y+2,this->geom.topleft.x+1);
        printl(" Use arrows to move your snake ");
        gotoyx(this->geom.topleft.y+3,this->geom.topleft.x+1);
        printl(" Press 'p' to pause the game ");
        gotoyx(this->geom.topleft.y+4,this->geom.topleft.x+1);
        printl(" If you hit the wall or snake, you die ");
        gotoyx(this->geom.topleft.y+5,this->geom.topleft.x+1);
        printl(" Have a nice day ");
        gotoyx(this->geom.topleft.y+6,this->geom.topleft.x+1);
        printl(" 'b' - back to menu");
    }
    else if(this->screen==3){
        gotoyx(this->geom.topleft.y+1,this->geom.topleft.x+1);
        printl(" You have %d points", this->snake.size());
        gotoyx(this->geom.topleft.y+2,this->geom.topleft.x+1);
        printl(" 'c' - to continue", this->snake.size());
        gotoyx(this->geom.topleft.y+3,this->geom.topleft.x+1);
        printl(" 'm' - end the game and go to menu", this->snake.size());
    }
    else if(this->screen==4){
        gotoyx(this->geom.topleft.y+1,this->geom.topleft.x+1);
        printl(" You die:(", this->snake.size());
        gotoyx(this->geom.topleft.y+2,this->geom.topleft.x+1);
        printl(" You have %d points", this->snake.size());
        gotoyx(this->geom.topleft.y+3,this->geom.topleft.x+1);
        printl(" 'm' - go to menu", this->snake.size());

    }
}

int i=0;

bool CSnake::handleEvent(int c){
    if(this->screen==0) {
        switch (c) {
            case KEY_UP:
                if (check_move(this->snake.at(0).x, this->snake.at(0).y - 1) == 1) {
					if(this->move.y!=1){
						this->move.x = 0;
						this->move.y = -1; 
					}
                }
                return true;
            case KEY_DOWN:
                if (check_move(this->snake.at(0).x, this->snake.at(0).y + 1) == 1) {
					if(this->move.y!=-1){
						this->move.x = 0;
						this->move.y = 1; 
					}
                }
                return true;
            case KEY_RIGHT:
                if (check_move(this->snake.at(0).x + 1, this->snake.at(0).y) == 1) {
					if(this->move.x!=-1){
						this->move.x = 1;  
						this->move.y = 0;
					}
                }
                return true;
            case KEY_LEFT:
                if (check_move(this->snake.at(0).x - 1, this->snake.at(0).y) == 1) {
					if(this->move.x!=1){
						this->move.x = -1; 
						this->move.y = 0;
					}			
                }
                return true;
            case 'p':
                this->screen=3;
                return true;
            default:
				if(i>=18){
					if (check_move(this->snake.at(0).x + this->move.x, this->snake.at(0).y+ this->move.y) == 1) {
						this->move_snake(this->snake.at(0).x + this->move.x, this->snake.at(0).y+ this->move.y);
					}
					i=0;
					return true;
				}
                i+=1;
				return false;
        };
    }
    else if(this->screen==1){
		if(CFramedWindow::handleEvent(c)==false){
			switch (c) {
				case 's':
					this->screen=0;
					return true;
				case 'i':
					this->screen=2;
					return true;
			}
			return false;
        }
        return true;
    }	
    else if(this->screen==2){
		if(CFramedWindow::handleEvent(c)==false){
			switch (c) {
				case 'b':
					this->screen=1;
					return true;
			}
			return false;
		}
		return true;
    }
    else if(this->screen==3){
		if(CFramedWindow::handleEvent(c)==false){
			switch (c) {
				case 'c':
					this->screen=0;
					return true;
				case 'm':
					this->screen=1;
					this->reset_snake();
					return true;
			}
			return false;
		}
		return true;
    }
    else if(this->screen==4){
		if(CFramedWindow::handleEvent(c)==false){
			switch (c) {
				case 'm':
					this->screen=1;
					this->reset_snake();
					return true;
			}
		}
		return true;
    }
    return false;

}

void CSnake::reset_snake(){
    this->snake.clear();
    this->snake.push_back(CPoint(1,1));
    this->snake_size=1;
    this->make_apple();
    this->move.x=0;
    this->move.y=1;
}

void CSnake::make_apple(){
	int x = rand()%(this->width-2)+1;
	int y = rand()%(this->height-2)+1;
	this->apple.x = x;
	this->apple.y = y;
}

void CSnake::move_snake(int x, int y){
	bool is_apple =0;
	if(this->snake[0].x==this->apple.x && this->snake[0].y==this->apple.y){
		is_apple=1;
		this->snake.push_back(CPoint(this->snake.at(snake_size-1).x,this->snake.at(snake_size-1).y));
	}
	
	for(int i=snake_size-1;i>0;i--){
		this->snake.at(i).x=this->snake.at(i-1).x;
		this->snake.at(i).y=this->snake.at(i-1).y;
	}
	this->snake.at(0).x=x;
	this->snake.at(0).y=y;
	if(is_apple){
		this->snake_size+=1;
		make_apple();
	}
}

bool CSnake::check_move(int x,int y){
	if(0==x){
        this->screen=4;
		return 0;
	}
	if(0==y){
        this->screen=4;
		return 0;
	}
	if(this->width==x+1){
        this->screen=4;
		return 0;
	}
	if(this->height==y+1){
        this->screen=4;
		return 0;
	}
	for(int i=0;i<this->snake_size-1;i++){
		if(this->snake.at(i).x==x &&this->snake.at(i).y==y){
            this->screen=4;
			return 0;
		} 
	}
	return 1;
}
