/*-----------------------------------------------------------------------------

	BasicData.h

	기본 데이터 클래스.

	2000.7.29. KJTINC

-----------------------------------------------------------------------------*/

#ifndef __BASICDATA_H__
#define __BASICDATA_H__

//----------------------------------------------------------------------------
// Point 클래스
//----------------------------------------------------------------------------
class Point
{
public:
	int		x, y;

	Point(int _x, int _y);
	Point();
	~Point();

	void	Set(int _x, int _y);
	void	operator=(int val);
};

//----------------------------------------------------------------------------
// Rect 클래스
//----------------------------------------------------------------------------
class Rect
{
public:
	int		x, y, w, h;

	Rect(int _x, int _y, int _w, int _h);
	Rect();
	~Rect();

	void	XY(int _x, int _y);
	void	WH(int _w, int _h);
	void	Set(int _x, int _y, int _w, int _h);
	void	operator=(int val);
	void	operator=(Point &point);
	void	operator+=(Point &point);
	
	bool	IsInRect(int _x, int _y) const;
	bool	IsInRect(const Point &point) const;

	int	Left() const;
	int	Right() const;
	int	Up() const;
	int	Down() const;
	//void	Left(int _x);
	//void	Right(int _x);
	//void	Up(int _y) const;
	//void	Down(int _y) const;
};

#endif