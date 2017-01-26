#include "polygons/convexfiller.h"
#include "lines/bresenham.h"
#include <iostream>
#ifdef __GLIB_CONVEXFILL__

namespace glib
{

convex_filler::convex_filler (DRAW draw) : ipolygoner(draw) {}

void convex_filler::draw (const poly_model* model) const
{
    std::vector<POINT> vertices = model->ysortindices();
    POINT top = vertices.front();
    POINT bot = vertices.back();
    size_t topy = top.second;
    size_t boty = bot.second;
    size_t dy = topy - boty + 1;
    std::vector<size_t> lxs(dy, -1);
    std::vector<size_t> rxs(dy, 0);
    
    // use bresen_liner for fast and accurate handling of bad slopes
    bresen_liner liner([&lxs, &rxs, topy, dy](int x, int y, unsigned int color)
    {
        size_t yidx = topy-y;
        if (yidx >= dy) return;
        // left indices goes towards the left
        if (lxs[yidx] > x) lxs[yidx] = x;
        // right indices goes towards the right
        if (rxs[yidx] < x) rxs[yidx] = x;
    });

    // populate lxs and rxs
    POINT last = model->get_v(0);
    size_t nvs = vertices.size();
    for (size_t i = 1; i < nvs; i++)
    {
        POINT pt = model->get_v(i);
        line_model line(last, pt);
        liner.draw(&line);
        last = pt;

        // we avoid discontinuations between edges
        // by explicitly entering vertices x coordinates
        size_t x = pt.first;
        size_t y = pt.second;
        size_t yidx = topy - y;
        if (lxs[yidx] > x) lxs[yidx] = x;
        if (rxs[yidx] < x) rxs[yidx] = x;
    }
    POINT pt = model->get_v(0);
    line_model line(last, pt);
    liner.draw(&line);
    for (size_t y = 0; y < dy; y++)
    {
        for (size_t x = lxs[y]; x <= rxs[y]; x++)
        {
            this->drawable_(x, topy-y, model->color_);
        }
    }
}

}

#endif