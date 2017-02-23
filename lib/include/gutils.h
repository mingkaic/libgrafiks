// 
// gutils.h
//
// purpose: provides common macros and utility functions
//

#include <cstdlib>
#include <cstdint>
#include <cmath>
#include <utility>
#include <functional>

#ifndef __GLIB_UTIL__
#define __GLIB_UTIL__

# define M_PI          3.141592653589793238462643383279502884L

namespace glib
{

#define DRAW std::function<void(int,int,int,unsigned)>

struct color
{
	color (unsigned base)
	{
		r = (base & 0x00ff0000) >> 16;
		g = (base & 0x0000ff00) >> 8;
		b = base & 0x000000ff;
	}

	color (uint8_t r, uint8_t g, uint8_t b) :
		r(r), g(g), b(b) {}

	explicit operator unsigned() const
	{
		return (0xff << 24) + (r << 16) + (g << 8) + b;
	}

	uint8_t r;
	uint8_t g;
	uint8_t b;
};

struct color_grad
{
	color_grad (unsigned base)
	{
		r = (double) ((base & 0x00ff0000) >> 16);
		g = (double) ((base & 0x0000ff00) >> 8);
		b = (double) (base & 0x000000ff);
	}

	color_grad (color c)
	{
		r = (double) c.r;
		g = (double) c.g;
		b = (double) c.b;
	}

	color_grad (double r, double g, double b) :
		r(r), g(g), b(b) {}

	operator unsigned() const
	{
		uint8_t red = (uint8_t) r;
		uint8_t green = (uint8_t) g;
		uint8_t blue = (uint8_t) b;
		return (0xff << 24) + (red << 16) + (green << 8) + blue;
	}

	const color_grad& operator += (color_grad& other)
	{
		r += other.r;
		g += other.g;
		b += other.b;
		return *this;
	}

	double r;
	double g;
	double b;
};

color_grad operator - (const color& lhs, const color& rhs);

color_grad operator / (const color_grad& lhs, double rhs);

color operator * (const color& lhs, double magnitude);

struct point
{
	point (void) : x(0), y(0), z(0) {}
	point (double x, double y, double z = 0) :
		x(x), y(y), z(z) {}

	double x;
	double y;
	double z;
	color basecolor = 0xffffffff; // white
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
