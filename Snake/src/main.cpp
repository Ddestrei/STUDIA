#include "winsys.h"
#include "snake.h"

int main ()
{
	/// Tworzy initializuje tło do którego będą dodawne kolejne pola
	CDesktop d;
	/// Tworzy okienko bez wypełnienia
	d.insert(new CInputLine (CRect (CPoint (0, 11), CPoint (15, 15))));
	/// Twory wypełnione okno
	d.insert(new CWindow (CRect (CPoint (0, 0), CPoint (15, 10)), '#'));
	/// Tworzy główne okno gry
	d.insert(new CSnake(CPoint (30, 5), CPoint (60, 20)));
	d.run ();
	return 0;
}
