#include "lib/include/gutils.h"

#ifdef __GLIB_UTIL__

namespace glib
{

void normal::normalize (void)
{
	double d = dist(*this);
	x /= d;
	y /= d;
	z /= d;
}

normal operator + (const normal& a, const normal& b)
{
	return normal{a.x + b.x, a.y + b.y, a.z + b.z};
}

normal operator - (const normal& a, const normal& b)
{
	return normal{a.x - b.x, a.y - b.y, a.z - b.z};
}

normal operator * (double scalar, const normal& b)
{
	return normal(b.x * scalar, b.y * scalar, b.z * scalar);
}

double dist (const normal& n)
{
	return std::sqrt(n.x * n.x + n.y * n.y + n.z * n.z);
}

double dot (const normal& n1, const normal& n2)
{
	return n1.x * n2.x + n1.y * n2.y + n1.z * n2.z;
}

normal cross (const normal& a, const normal& b)
{
	return normal(
		a.y * b.z - a.z * b.y,
		a.z * b.x - a.x * b.z,
		a.x * b.y - a.y * b.x);
}

point cross (const point& lhs, const point& rhs)
{
	return {
		lhs.getY() * rhs.getZ() - rhs.getY() * lhs.getZ(),
		lhs.getZ() * rhs.getX() - rhs.getZ() * lhs.getX(),
		lhs.getX() * rhs.getY() - rhs.getX() * lhs.getY()
	};
}

double dot (const point& lhs, const point& rhs)
{
	return lhs.getX() * rhs.getX() + lhs.getY() * rhs.getY() + lhs.getZ() * rhs.getZ();
}

point operator * (const point& lhs, const point& rhs)
{
	return {lhs.getX() * rhs.getX(),
			lhs.getY() * rhs.getY(),
			lhs.getZ() * rhs.getZ()};
}

point operator + (const point& lhs, const point& rhs)
{
	return {lhs.getX() + rhs.getX(),
			lhs.getY() + rhs.getY(),
			lhs.getZ() + rhs.getZ()};
}

point operator - (const point& lhs, const point& rhs)
{
	return {lhs.getX() - rhs.getX(),
			lhs.getY() - rhs.getY(),
			lhs.getZ() - rhs.getZ()};
}

bool operator < (const point& lhs, double limit)
{
	return lhs.getX() < limit || lhs.getY() < limit || lhs.getZ() < limit;
}

inline double norm2(const point& p)
{
	return p.getX() * p.getX() + p.getY() * p.getY() + p.getZ() * p.getZ();
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
    if (pt1.getX() > pt2.getX())
    {
        point buffer = std::move(pt1);
        pt1 = std::move(pt2);
        pt2 = std::move(buffer);
    }
}

double slope (point pt1, point pt2)
{
    inorder(pt1, pt2);
    if (pt2.getX() - pt1.getX() == 0)
    {
        return std::numeric_limits<double>::max(); // return yuge value
    }
    return (pt2.getY() - pt1.getY()) / (pt2.getX() - pt1.getX());
}

double slope_tangent (point pt1, point pt2)
{
    inorder(pt1, pt2);
    if (pt2.getY() - pt1.getY() == 0)
    {
        return std::numeric_limits<double>::max(); // return yuge value
    }
    return (pt2.getX() - pt1.getX()) / (pt2.getY() - pt1.getY());
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
