#include <limits>

#include "lib/include/polygons/convexfiller.h"
#include "lib/include/lines/bresenham.h"

#ifdef __GLIB_CONVEXFILL__

namespace glib
{

#define TOP std::numeric_limits<double>::max()

convex_filler::convex_filler (DRAW draw) : ipolygoner(draw) {}

void convex_filler::draw (const poly_model* model) const
{
    std::vector<point> vertices = model->ysortindices();
    point top = vertices.front();
    point bot = vertices.back();
	double topy = top.y;
    double boty = bot.y;
    size_t dy = (int) topy - (int) boty + 1;
    std::vector<point> lxs(dy, {TOP, 0, 0});
    std::vector<point> rxs(dy, {0, 0, 0});
    
    // use bresen_liner for fast and accurate handling of bad slopes
    bresen_liner liner([&lxs, &rxs, topy, dy](int x, int y, int z, unsigned int color)
    {
		assert(y <= std::ceil(topy));
		point p = {(double)x, (double)y, (double)z};
		p.basecolor = color;
        size_t yidx = (int) topy-y;
        if (yidx >= dy) return;
        // left indices goes towards the left
        if (lxs[yidx].x >= x) lxs[yidx] = p;
        // right indices goes towards the right
        if (rxs[yidx].x <= x) rxs[yidx] = p;
    });

    // populate lxs and rxs
	size_t nvs = vertices.size();
    point last = model->get_v(nvs-1);
    for (size_t i = 0; i < nvs; i++)
    {
        point pt = model->get_v(i);
        line_model line(last, pt);
        liner.draw(&line);

        // we avoid discontinuations between edges
        // by explicitly entering vertices x coordinate
        double x = pt.x;
		int y = pt.y;

        size_t yidx = (int) topy - y;

        if (lxs[yidx].x > x) lxs[yidx] = pt;
        if (rxs[yidx].x < x) rxs[yidx] = pt;

		last = pt;
    }
    for (size_t y = 0; y < dy; y++)
    {
    	int lx = lxs[y].x;
		int rx = rxs[y].x;

		double dx = rxs[y].x - lxs[y].x;
    	double dz = rxs[y].z - lxs[y].z;
		color_grad dc = rxs[y].basecolor - lxs[y].basecolor;

		double mz = dz / dx;
		color_grad mc = dc / dx;

		double basez = lxs[y].z;
		color_grad basec = lxs[y].basecolor;

        for (int x = lx; x <= rx; x++)
        {
            this->drawable_(x, topy-y, basez, basec);
			basez += mz;
			basec += mc;
        }
    }
}

}

#endif /* __GLIB_CONVEXFILL__ */
