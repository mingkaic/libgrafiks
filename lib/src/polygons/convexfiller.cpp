#include "polygons/convexfiller.h"
#include "lines/bresenham.h"

#ifdef __GLIB_CONVEXFILL__

namespace glib
{

convex_filler::convex_filler (DRAW draw) : ipolygoner(draw) {}

void convex_filler::draw (const poly_model* model) const
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
    point last = model->get_v(0);
    size_t nvs = vertices.size();
    for (size_t i = 1; i < nvs; i++)
    {
        point pt = model->get_v(i);
        line_model line(last, pt);
        liner.draw(&line);
        last = pt;

        // we avoid discontinuations between edges
        // by explicitly entering vertices x coordinate
        size_t x = pt.x;
        size_t y = pt.y;
        size_t yidx = topy - y;
        if (lxs[yidx] > x) lxs[yidx] = x;
        if (rxs[yidx] < x) rxs[yidx] = x;
    }
    point pt = model->get_v(0);
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

#endif /* __GLIB_CONVEXFILL__ */