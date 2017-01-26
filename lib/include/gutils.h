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

#define POINT std::pair<double, double>
#define DRAW std::function<void(int, int,unsigned int)>

// sort by x such that pt1 has the lower x value
void inorder (POINT& pt1, POINT& pt2);

double slope (POINT pt1, POINT pt2);

double slope_tangent (POINT pt1, POINT pt2);

unsigned int opacity_transform (unsigned int color, double opacity);

unsigned int higher_intensity (unsigned int c1, unsigned int c2);

struct coord_transform
{
    double data_[2][2];

    void forward (double& x, double& y) const;
    void backward (double& x, double& y) const;
};

}

#endif /* __GLIB_UTIL__ */
