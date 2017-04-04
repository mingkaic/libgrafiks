#include "lib/include/lines/bresenham.h"

#ifdef __GLIB_BRESEN__

namespace glib
{

bresen_liner::bresen_liner (DRAW draw) : iliner(draw) {}

void bresen_liner::draw (const line_model* model) const
{
	const transformation& forward = this->octantize(*model);
	transformation backward = forward.transpose();
	double dummyz = 1;

	point to = model->get_v(1);
	point from = model->get_v(0);
	lerper* lrrr;
	if (model->face_color_) // FLAT shading
	{
		lrrr = new lerper(to, from);
	}
	else if (model->shader_) // PHONG shading
	{
		lrrr = new normal_lerper(to, from);
	}
	else // GOURAUD shading
	{
		lrrr = new color_lerper(to, from);
	}

	double dz = model->dz();
	color_grad dc = model->dc();

	forward.mul(lrrr->dx_, lrrr->dy_, dummyz); // transform to oct1

	double mz = lrrr->dz_ / lrrr->dx_;

	point origin = model->get_v(0);
	double centerx = origin.getX();
	double centery = origin.getY();
	double centerz = origin.getZ();
	double centerzp = origin.zp;
	color_grad centercolor = (unsigned) origin.basecolor;
	normal centern = origin.n;

	double twodx = 2*lrrr->dx_;
	double twody = 2*lrrr->dy_;
	double err = twody - lrrr->dx_;

	double t2 = twody - twodx;
	this->drawable_(centerx, centery, centerz, centerzp, centercolor, centern);
	double y = 0;
	for (size_t x = 1; x <= lrrr->dx_; x++)
	{
		double fakey = 0, fakex = x;
		lrrr->step(x-1, fakex, fakey, centerz, centerzp, centercolor, centern);
		if (err >= 0)
		{
			err += t2;
			y++;
		}
		else
		{
			err += twody;
		}
		double xi = x, yi = y;
		backward.mul(xi, yi, dummyz);

		this->drawable_(centerx + xi, centery + yi, centerz, centerzp, centercolor, centern);
	}
	delete lrrr;
}

}

#endif
