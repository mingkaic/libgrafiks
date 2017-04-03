//
// Created by Mingkai Chen on 2017-02-18.
//

#include "lib/include/polygons/convexwirer.h"
#include "lib/include/lines/bresenham.h"

#ifdef LIBGRAFIKS_CONVEXWIRER_HPP

namespace glib
{

convex_wirer::convex_wirer (DRAW draw) : ipolygoner(draw) {}

void convex_wirer::draw (const poly_model* model) const
{
	// use bresen_liner for fast and accurate handling of bad slopes
	bresen_liner liner(this->drawable_);

	// populate lxs and rxs
	size_t nvs = model->n_vertices();
	point last = model->get_v(nvs-1);

	for (size_t i = 0; i < nvs; i++)
	{
		point pt = model->get_v(i);
		line_model line(last, pt);
		liner.draw(&line);
		last = pt;
	}
}

}

#endif
