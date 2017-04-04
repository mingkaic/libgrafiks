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

struct normal
{
	normal (void) : x(0), y(0), z(0) {}
	normal (double x, double y, double z = 0) :
		x(x), y(y), z(z) {}

	void normalize (void);

	double x;
	double y;
	double z;
};

#define DRAW std::function<void(int,int,double,double,unsigned,glib::normal&)>

normal operator + (const normal& a, const normal& b);

normal operator - (const normal& a, const normal& b);

normal operator * (double scalar, const normal& b);

double dist (const normal& n);

double dot (const normal& n1, const normal& n2);

normal cross (const normal& a, const normal& b);

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
	double zp;
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

struct lerper
{
	enum ARG { X, Y, Z };

	lerper (point& to, point& from)
	{
		dx_ = to.getX() - from.getX();
		dy_ = to.getY() - from.getY();
		dz_ = to.getZ() - from.getZ();
		dzp_ = to.zp - from.zp;
	}

	virtual double step (double arg, double& x, double& y, double& z, double& zp, color_grad& c, normal& n, ARG signal = X)
	{
		double d;
		switch (signal)
		{
			case X:
			{
				d = (x - arg) / dx_;
				y += d * dy_;
				z += d * dz_;
			}
				break;
			case Y:
			{
				d = (y - arg) / dy_;
				x += d * dx_;
				z += d * dz_;
			}
				break;
			case Z:
			{
				d = (z - arg) / dz_;
				x += d * dx_;
				y += d * dy_;
			}
				break;
		}
		zp += d * dzp_;
		return d;
	}

	double dx_;
	double dy_;
	double dz_;
	double dzp_;
};

struct color_lerper : public lerper
{
	color_lerper (point& to, point& from) :
		lerper(to, from), dc_(0)
	{
		dc_ = to.basecolor - from.basecolor;
	}

	virtual double step (double arg, double& x, double& y, double& z, double& zp, color_grad& c, normal& n, ARG signal = X)
	{
		double d = lerper::step(arg, x, y, z, zp, c, n, signal);
		c += dc_ * d;
		return d;
	}

	color_grad dc_;
};

struct normal_lerper : public color_lerper
{
	normal_lerper (point& to, point& from) :
		color_lerper(to, from)
	{
		dn_ = to.n - from.n;
	}

	virtual double step (double arg, double& x, double& y, double& z, double& zp, color_grad& c, normal& n, ARG signal = X)
	{
		double d = color_lerper::step(arg, x, y, z, zp, c, n, signal);
		n = n + d * dn_;
		return d;
	}

	normal dn_;
};

}

#endif /* __GLIB_UTIL__ */
