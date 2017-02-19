//
// Created by Mingkai Chen on 2017-02-18.
//

#include "polygons/convexwirer.h"
#include "lines/bresenham.h"

#ifdef LIBGRAFIKS_CONVEXWIRER_HPP

namespace glib
{

convex_wirer::convex_wirer (DRAW draw) : ipolygoner(draw) {}

void convex_wirer::draw (const poly_model* model) const
{
    std::vector<point> vertices = model->ysortindices();
    point top = vertices.front();
    point bot = vertices.back();
    size_t topy = top.y;
    size_t boty = bot.y;
    size_t dy = topy - boty + 1;
    std::vector<size_t> lxs(dy, -1);
    std::vector<size_t> rxs(dy, 0);

    // use bresen_liner for fast and accurate handling of bad slopes
    bresen_liner liner(this->drawable_);

    // populate lxs and rxs
    point last = model->get_v(0);
    size_t nvs = vertices.size();
    for (size_t i = 1; i < nvs; i++)
    {
        point pt = model->get_v(i);
        line_model line(last, pt, model->color_);
        liner.draw(&line);
        last = pt;

        // we avoid discontinuations between edges
        // by explicitly drawing vertices x coordinate
        double x = pt.x;
        double y = pt.y;
        this->drawable_(x, y, model->color_);
    }
}

}

#endif
