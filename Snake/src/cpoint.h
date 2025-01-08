#ifndef __CPOINT_H__
#define __CPOINT_H__

#include <list>

using namespace std;

struct CPoint
{
  int x;
  int y;
  CPoint(int _x=0, int _y=0): x(_x), y(_y) {};
  CPoint& operator+=(const CPoint& delta)
  {
    x+=delta.x;
    y+=delta.y;
    return *this;
  }
};

/// Zawiera dane o prostokącie 
struct CRect
{
	/// lewy górny punk prostokąta 
	CPoint topleft;
	/// wektor przesunięcia topleft do przeciwległego wierzchołka.
	CPoint size;
	CRect(CPoint t1=CPoint(), CPoint s=CPoint()):
    topleft(t1), size(s) {};
};

#endif
