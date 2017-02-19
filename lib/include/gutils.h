// 
// gutils.h
//
// purpose: provides common macros and utility functions
//

#include <utility>
#include <functional>

#ifndef __GLIB_UTIL__
#define __GLIB_UTIL__

namespace glib
{

#define DRAW std::function<void(int, int,unsigned int)>

struct point
{
	point (double x, double y, double z = 0) :
		x(x), y(y), z(z) {}

	double x;
	double y;
	double z;
};

// sort by x such that pt1 has the lower x value
void inorder (point& pt1, point& pt2);

double slope (point pt1, point pt2);

double slope_tangent (point pt1, point pt2);

unsigned int opacity_transform (unsigned int color, double opacity);

unsigned int higher_intensity (unsigned int c1, unsigned int c2);

}

#endif /* __GLIB_UTIL__ */
