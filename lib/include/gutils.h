// 
// gutils.h
//
// purpose: provides common macros and utility functions
//

#include <cstdint>
#include <cmath>
#include <utility>
#include <functional>
#include <iostream>

#include "lib/include/light/color.h"

#ifndef __GLIB_UTIL__
#define __GLIB_UTIL__

#ifndef M_PI
#define M_PI          3.141592653589793238462643383279502884L
#endif // M_PI

namespace glib
{

#define DRAW std::function<void(int,int,int,unsigned)>

struct normal
{
	normal (void) : x(0), y(0), z(0) {}
	normal (double x, double y, double z = 0) :
		x(x), y(y), z(z) {}

	double x;
	double y;
	double z;
};

normal operator - (const normal& a, const normal& b);

double dot (const normal& n1, const normal& n2);

struct point
{
	point (void) : x(0), y(0), z(0) {}
	point (double x, double y, double z = 0) :
		x(x), y(y), z(z) {}
	point (double x, double y, double z, double h) :
		x(x), y(y), z(z), h(h) {}

	double x;
	double y;
	double z;
	double h = 1;

	double getX (void) const { return x / h; }
	double getY (void) const { return y / h; }
	double getZ (void) const { return z / h; }
	normal n;
	color basecolor = 0xffffffff; // white todo: make flag value or optional
};

struct plane
{
	plane (normal pt, normal norm) :
		pt_(pt), norm_(norm) {}

	normal pt_;
	normal norm_;
};

point cross (const point& lhs, const point& rhs);

double dot (const point& lhs, const point& rhs);

point operator * (const point& lhs, const point& rhs);

point operator + (const point& lhs, const point& rhs);

point operator - (const point& lhs, const point& rhs);

bool operator < (const point& lhs, double limit);

bool intersection (
	std::pair<point, point> l1,
	std::pair<point, point> l2,
	point& res);

// sort by x such that pt1 has the lower x value
void inorder (point& pt1, point& pt2);

double slope (point pt1, point pt2);

double slope_tangent (point pt1, point pt2);

unsigned int opacity_transform (unsigned int c, double opacity);

unsigned int higher_intensity (unsigned int c1, unsigned int c2);

}

#endif /* __GLIB_UTIL__ */
