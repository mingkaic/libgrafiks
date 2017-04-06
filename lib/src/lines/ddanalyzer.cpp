#include "lib/include/lines/ddanalyzer.h"

#ifdef __GLIB_DDA__

namespace glib
{

dda_liner::dda_liner (DRAW draw) : iliner(draw) {}

void dda_liner::draw (const line_model* model) const
{
	const transformation& forward = this->octantize(*model);
	transformation backward = forward.transpose();
	double dummyz = 1;

	point to = model->get_v(1);
	point from = model->get_v(0);
	color_lerper lrrr(to, from);
	double dx = lrrr.dx_;

	color_grad dc = model->dc();

	forward.mul(lrrr.dx_, lrrr.dy_, dummyz); // transform to oct1

	point origin = model->get_v(0);
	double centerx = origin.getX();
	double centery = origin.getY();
	double centerz = origin.getZ();
	double centerzp = origin.zp;
	color_grad centercolor = origin.basecolor;
	normal centern = origin.n;

	double diff = centerx - std::round(centerx);
	centerx = centerx - diff; // round x
	centery += diff * lrrr.dy_ / lrrr.dx_; // move y to match rounded x

	double y = 0;
	double xi = 0, yi = std::round(y);
	backward.mul(xi, yi, dummyz);
	this->drawable_(centerx + xi, centery + yi, centerz,
		centerzp, centercolor, centern);

	for (double x = 1; x <= dx; ++x)
	{
		xi = x;
		yi = std::round(y);
		backward.mul(xi, yi, dummyz);
		this->drawable_(centerx + xi, centery + yi, centerz,
			centerzp, centercolor, centern);
		lrrr.step(x-1, x, y, centerz, centerzp, centercolor, centern);
	}
}

}

#endif
