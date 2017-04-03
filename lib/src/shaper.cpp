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

	if (poly_model* pm = dynamic_cast<poly_model*>(model_))
	{
		// backface culling
		if (!pm->cclockwise())
		{
			return;
		}

		pm->trans_points(
			[this, pm, &trans, &Ktrans](point& c)
			{
				c.n.normalize();
				c.basecolor = sources_(normal(c.getX(), c.getY(), c.getZ()),
					c.n, trans, Ktrans, pm->kd_, pm->ks_, pm->p_);
			});
		switch(shad_)
		{
			case FLAT_SHAD:
				break;
			case GOURAUD_SHAD:
				break;
			case PHONG_SHAD:
				break;
		}

	}

	shaper_->draw(model_);
}

}

#endif