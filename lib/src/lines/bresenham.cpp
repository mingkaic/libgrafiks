#include "lines/bresenham.h"

#ifdef __GLIB_BRESEN__

namespace glib
{

bresen_liner::bresen_liner (DRAW draw) : iliner(draw) {}

void bresen_liner::draw (const line_model* model) const
{
	const transformation& forward = this->octantize(*model);
	transformation backward = forward.transpose();
	double dummyz = 1;

    double dx = model->dx();
    double dy = model->dy();
    forward.mul(dx, dy, dummyz); // transform to oct1

    point origin = model->get_v(0);
    double centerx = origin.x;
    double centery = origin.y;
    double twodx = 2*dx;
    double twody = 2*dy;
    double err = twody - dx;
    double t2 = twody - twodx;
    this->drawable_(centerx, centery, model->color_);
    double y = 0;
    for (size_t x = 1; x <= dx; x++)
    {
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
        this->drawable_(centerx + xi, centery + yi, model->color_);
    }
}

}

#endif