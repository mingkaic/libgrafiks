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

	double dx = model->dx();
	double dy = model->dy();
	double dz = model->dz();
	color_grad dc = model->dc();

	forward.mul(dx, dy, dummyz); // transform to oct1

	double mz = dz / dx;
	color_grad mc = dc / dx;

	double m = dy / dx;

	point origin = model->get_v(0);
	double centerx = origin.getX();
	double centery = origin.getY();
	double centerz = origin.getZ();
	color_grad centercolor = origin.basecolor;
	normal centern = origin.n;

	double diff = centerx - std::round(centerx);
	centerx = centerx - diff; // round x
	centery += diff * m; // move y to match rounded x

	double y = 0;
	for (size_t x = 0; x <= dx; ++x)
	{
		double xi = x, yi = std::round(y);
		backward.mul(xi, yi, dummyz);
		this->drawable_(centerx + xi, centery + yi, centerz,
			centerz, centercolor, centern);
		y += m;
		centerz += mz;
		centercolor += mc;
	}
}

}

#endif
