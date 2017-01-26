#include "gutils.h"
#include <iostream>
#ifdef __GLIB_UTIL__

namespace glib
{

void inorder (POINT& pt1, POINT& pt2)
{
    if (pt1.first > pt2.first)
    {
        POINT buffer = std::move(pt1);
        pt1 = std::move(pt2);
        pt2 = std::move(buffer);
    }
}

double slope (POINT pt1, POINT pt2)
{
    inorder(pt1, pt2);
    if (pt2.first - pt1.first == 0)
    {
        return std::numeric_limits<double>::max(); // return yuge value
    }
    return (pt2.second - pt1.second) / (pt2.first - pt1.first);
}

double slope_tangent (POINT pt1, POINT pt2)
{
    inorder(pt1, pt2);
    if (pt2.second - pt1.second == 0)
    {
        return std::numeric_limits<double>::max(); // return yuge value
    }
    return (pt2.first - pt1.first) / (pt2.second - pt1.second);
}

void rgb_unpack (unsigned int color, uint8_t& red, uint8_t& green, uint8_t& blue)
{
    red = (color & 0x00ff0000) >> 16;
    green = (color & 0x0000ff00) >> 8;
    blue = color & 0x000000ff;
}

unsigned int opacity_transform (unsigned int color, double opacity)
{
    uint8_t r, g, b;
    rgb_unpack(color, r, g, b);
    r *= opacity;
    g *= opacity;
    b *= opacity;
    color = (0xff << 24) + (r << 16) + (g << 8) + b;
    return color;
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

void coord_transform::forward (double& x, double& y) const
{
    double outx = x * data_[0][0] + y * data_[0][1];
    double outy = x * data_[1][0] + y * data_[1][1];
    x = outx;
    y = outy;
}

void coord_transform::backward (double& x, double& y) const
{
    double outx = x * data_[0][0] + y * data_[1][0];
    double outy = x * data_[0][1] + y * data_[1][1];
    x = outx;
    y = outy;
}

}

#endif