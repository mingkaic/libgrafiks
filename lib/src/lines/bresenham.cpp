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

//	double dx = model->dx();
//	double dy = model->dy();
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

//	forward.mul(dx, dy, dummyz); // transform to oct1
	forward.mul(lrrr->dx_, lrrr->dy_, dummyz); // transform to oct1

//	double mz = dz / dx;
//	color_grad mc = dc / dx;
	double mz = lrrr->dz_ / lrrr->dx_;

	point origin = model->get_v(0);
	double centerx = origin.getX();
	double centery = origin.getY();
	double centerz = origin.getZ();
	color_grad centercolor = (unsigned) origin.basecolor;
	normal centern = origin.n;

//	double twodx = 2*dx;
//	double twody = 2*dy;
//	double err = twody - dx;
	double twodx = 2*lrrr->dx_;
	double twody = 2*lrrr->dy_;
	double err = twody - lrrr->dx_;

	double t2 = twody - twodx;
	this->drawable_(centerx, centery, centerz, centercolor, centern);
	double y = 0;
//	for (size_t x = 1; x <= dx; x++)
	for (size_t x = 1; x <= lrrr->dx_; x++)
	{
//		centerz += mz;
//		centercolor += mc;
		double fakey = 0, fakex = x;
		lrrr->step(x-1, fakex, fakey, centerz, centercolor, centern);
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

		this->drawable_(centerx + xi, centery + yi, centerz, centercolor, centern);
	}
	delete lrrr;
}

}

#endif
