#include "common.h"

#ifdef TESTCOMMONS

std::vector<glib::point> starburst_vertices (size_t centerx, size_t centery, size_t length, size_t nlines)
{
	glib::point origin((double) centerx, (double) centery);
	double theta = 0;
	double dtheta = 2*M_PI/nlines;
	std::vector<glib::point> pts;
	for (size_t i = 0; i < nlines; i++)
	{
		double x = centerx + length * std::cos(theta);
		double y = centery + length * std::sin(theta);
		theta += dtheta;
		pts.push_back(glib::point(x, y));
	}
	return pts;
}

#endif
