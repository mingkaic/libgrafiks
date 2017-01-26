#include "lines/bresenham.h"

#ifdef __GLIB_BRESEN__

namespace glib
{

bresen_liner::bresen_liner (DRAW draw) : iliner(draw) {}

void bresen_liner::draw (const line_model* model) const
{
    const coord_transform& transform = this->octantize(*model);
    double dx = model->dx();
    double dy = model->dy();
    transform.forward(dx, dy); // transform to oct1

    POINT origin = model->get_v(0);
    double centerx = origin.first;
    double centery = origin.second;
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
        transform.backward(xi, yi);
        this->drawable_(centerx + xi, centery + yi, model->color_);
    }
}

}

#endif