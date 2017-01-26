#include "lines/ddanalyzer.h"

#ifdef __GLIB_DDA__

namespace glib
{

dda_liner::dda_liner (DRAW draw) : iliner(draw) {}

void dda_liner::draw (const line_model* model) const
{
    const coord_transform& transform = this->octantize(*model);
    double dx = model->dx();
    double dy = model->dy();
    transform.forward(dx, dy); // transform to oct1
    double m = dy / dx;

    POINT origin = model->get_v(0);
    double centerx = origin.first;
    double centery = origin.second;
    double diff = centerx - std::round(centerx);
    centerx = centerx - diff; // round x
    centery += diff * m; // move y to match rounded x

    double y = 0;
    for (size_t x = 0; x <= dx; ++x)
    {
        double xi = x, yi = std::round(y);
        transform.backward(xi, yi);
        this->drawable_(centerx + xi, centery + yi, model->color_);
        y += m;
    }
}

}

#endif