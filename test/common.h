#include <cassert>
#include "gutils.h"

#ifndef TESTCOMMONS
#define TESTCOMMONS

namespace mingtest
{

std::vector<POINT> starburst_vertices (size_t centerx, size_t centery, size_t length, size_t nlines)
{
    POINT origin(centerx, centery);
    double theta = 0;
    double dtheta = 2*M_PI/nlines;
    std::vector<POINT> pts;
    for (size_t i = 0; i < nlines; i++)
    {
        double x = centerx + length * std::cos(theta);
        double y = centery + length * std::sin(theta);
        theta += dtheta;
        pts.push_back(POINT(x, y));
    }
    return pts;
}

}

#endif /* TESTCOMMONS */