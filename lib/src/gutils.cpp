#include "lib/include/gutils.h"

#ifdef __GLIB_UTIL__

namespace glib
{

color_grad operator - (const color& lhs, const color& rhs)
{
    return color_grad{
		(double) lhs.r - (double) rhs.r,
		(double) lhs.g - (double) rhs.g,
		(double) lhs.b - (double) rhs.b
    };
}

color_grad operator / (const color_grad& lhs, double rhs)
{
	return color_grad{
		lhs.r / rhs,
		lhs.g / rhs,
		lhs.b / rhs
	};
}

color operator * (const color& lhs, double magnitude)
{
	return color{
		(uint8_t) (lhs.r * magnitude),
		(uint8_t) (lhs.g * magnitude),
		(uint8_t) (lhs.b * magnitude)
	};
}

point cross (const point& lhs, const point& rhs)
{
	return {
		lhs.y * rhs.z - rhs.y * lhs.z,
		lhs.z * rhs.x - rhs.z * lhs.x,
		lhs.x *  rhs.y - rhs.x * lhs.y
	};
}

double dot (const point& lhs, const point& rhs)
{
	return lhs.x * rhs.x + lhs.y * rhs.y + lhs.z * rhs.z;
}

point operator * (const point& lhs, const point& rhs)
{
	return {lhs.x * rhs.x,
			lhs.y * rhs.y,
			lhs.z * rhs.z};
}

point operator + (const point& lhs, const point& rhs)
{
	return {lhs.x + rhs.x,
			lhs.y + rhs.y,
			lhs.z + rhs.z};
}

point operator - (const point& lhs, const point& rhs)
{
	return {lhs.x - rhs.x,
			lhs.y - rhs.y,
			lhs.z - rhs.z};
}

bool operator < (const point& lhs, double limit)
{
	return lhs.x < limit || lhs.y < limit || lhs.z < limit;
}

inline double norm2(const point& p)
{
	return p.x * p.x + p.y * p.y + p.z * p.z;
}

inline double norm( const point& p )
{
	return std::sqrt(norm2(p));
}

bool intersection (
	std::pair<point, point> l1,
	std::pair<point, point> l2,
	point& res) {
	point da = l1.second - l1.first;
	point db = l2.second - l1.first;
	point dc = l2.first - l1.first;

	if (dot(dc, cross(da, db)) == 0) // lines are coplanar
	{
		point cdadb = cross(da, db);
		double s = dot(cross(dc, db), cdadb)
					/ norm2(cdadb);
		if (s >= 0 && s <= 1) {
			res = l1.first + da * point{s, s, s};
			return true;
		}
	}
	return false;
}

void inorder (point& pt1, point& pt2)
{
    if (pt1.x > pt2.x)
    {
        point buffer = std::move(pt1);
        pt1 = std::move(pt2);
        pt2 = std::move(buffer);
    }
}

double slope (point pt1, point pt2)
{
    inorder(pt1, pt2);
    if (pt2.x - pt1.x == 0)
    {
        return std::numeric_limits<double>::max(); // return yuge value
    }
    return (pt2.y - pt1.y) / (pt2.x - pt1.x);
}

double slope_tangent (point pt1, point pt2)
{
    inorder(pt1, pt2);
    if (pt2.y - pt1.y == 0)
    {
        return std::numeric_limits<double>::max(); // return yuge value
    }
    return (pt2.x - pt1.x) / (pt2.y - pt1.y);
}

void rgb_unpack (unsigned int c, uint8_t& red, uint8_t& green, uint8_t& blue)
{
    red = (c & 0x00ff0000) >> 16;
    green = (c & 0x0000ff00) >> 8;
    blue = c & 0x000000ff;
}

unsigned int opacity_transform (unsigned int c, double opacity)
{
    uint8_t r, g, b;
    rgb_unpack(c, r, g, b);
    r = r * opacity;
    g = g * opacity;
    b = b * opacity;
    c = (0xff << 24) + (r << 16) + (g << 8) + b;
    return c;
}

// return color of highest average intensity in all channels
unsigned int higher_intensity (unsigned int c1, unsigned int c2)
{
    uint8_t r1, g1, b1, r2, g2, b2;
    rgb_unpack(c1, r1, g1, b1);
    rgb_unpack(c2, r2, g2, b2);
    signed diffr = r1 - r2;
    signed diffg = g1 - g2;
    signed diffb = b1 - b2;
    if (diffr + diffg + diffb < 0) // c2 > c1
    {
        return c2;
    }
    return c1;
}

}

#endif
