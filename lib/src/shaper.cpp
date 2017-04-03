//
// Created by Mingkai Chen on 2017-03-31.
//

#include "lib/include/shaper.h"
#include "lib/include/polygons/polygon.h"

#ifdef __GLIB_SHAPE__

namespace glib
{

void shape_render::run (std::vector<const transformation*> trans, const camera_transform* Ktrans) {
	assert(shaper_ && model_);

	color face(0);
	if (poly_model* pm = dynamic_cast<poly_model*>(model_))
	{
		LIGHTCOLOR lightpt = [this, pm, &trans, &Ktrans]
		(double x, double y, double z, color_grad c, normal n) -> unsigned
		{
			c.r /= 255;
			c.g /= 255;
			c.b /= 255;
			return (unsigned) sources_(normal(x, y, z),
				n, trans, Ktrans, c, pm->ks_, pm->p_);
		};
		color_grad total(0);
		// backface culling
		if (!pm->cclockwise())
		{
			return;
		}

		if (FLAT_SHAD == shad_ || GOURAUD_SHAD == shad_)
		{
			pm->trans_points(
			[&total, pm, &lightpt](point& c)
			{
				color_grad cg = c.basecolor = lightpt(c.getX(), c.getY(), c.getZ(), c.basecolor, c.n);
				total += cg / (double) pm->n_vertices();
			});
		}
		if (FLAT_SHAD == shad_)
		{
			face = (unsigned) total;
			pm->face_color_ = &face;
		}
		else if (PHONG_SHAD == shad_)
		{
			pm->shader_ = lightpt;
		}
	}
	shaper_->draw(model_);
}

}

#endif