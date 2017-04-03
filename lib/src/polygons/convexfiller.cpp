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
	double topy = top.getY();
    double boty = bot.getY();
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
        if (lxs[yidx].getX() >= x) lxs[yidx] = p;
        // right indices goes towards the right
        if (rxs[yidx].getX() <= x) rxs[yidx] = p;
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
		double x = pt.getX();
		int y = pt.getY();

        size_t yidx = (int) topy - y;
        if (lxs[yidx].getX() > x) lxs[yidx] = pt;
        if (rxs[yidx].getX() < x) rxs[yidx] = pt;

		last = pt;
    }
    std::function<double(std::vector<point>&,size_t)> correction =
		[](std::vector<point>& xarr, size_t y)
		{
			point p1 = xarr[y-2];
			point p2 = xarr[y-1];
			double basex = p2.getX();
			double basez = p2.getZ();
			color_grad basec = p2.basecolor;

			double dx = p1.getX() - basex;
			double dz = p1.getZ() - basez;
			color_grad dc = p1.basecolor - p2.basecolor;
			basex += dx;
			basez += dz;
			basec += dc;
			xarr[y] = {basex, (double) y, basez};
			xarr[y].basecolor = (unsigned) basec;
			return basex;
		};
    for (size_t y = 0; y < dy; y++)
    {
    	double lx = lxs[y].getX();
		double rx = rxs[y].getX();

		if (lx > rx)
		{
			// error: correct by lerp
			if (y < 2)
			{
				continue;
			}
			lx = correction(lxs, y);
			rx = correction(rxs, y);
		}

		double dx = rxs[y].getX() - lxs[y].getX();
    	double dz = rxs[y].getZ() - lxs[y].getZ();
		color_grad dc = rxs[y].basecolor - lxs[y].basecolor;

		double mz = dz / dx;
		color_grad mc = dc / dx;

		double basez = lxs[y].getZ();
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
