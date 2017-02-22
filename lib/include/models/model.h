// 
// model.h
//
// purpose: provide an interface to modeling polygons
//

#include <vector>

#include "gutils.h"
#include "models/transformation.h"

#ifndef __GLIB_MODEL__
#define __GLIB_MODEL__

namespace glib
{

class shape_model
{
public:
	point get_v (size_t idx) const
	{
		return vertices_[idx];
	}

	void transform (const transformation& trans)
	{
		for (point& p : vertices_)
		{
			trans.mul(p.x, p.y, p.z);
		}
	}

	size_t n_vertices (void) const
	{
		return vertices_.size();
	}

	// normal direction points inward
	// (normal must be unit)
	// vertices outside the designated direction "crosses" the plane
	// return true if model is culled
	bool clip_plane (point center, point normal)
	{
		point centerdiff = center * normal;
		bool allcrosses = true;
		size_t nv = vertices_.size();
		point last = vertices_[nv-1];
		for (size_t i = 0; i < nv; i++)
		{
			bool crosses = false;
			point p = vertices_[i];

			point dp = p * normal - centerdiff;
			point dl = last * normal - centerdiff;
			// L1 = p, last
			// L2 = p', last'
			point pp = p + dp;
			point pl = last + dl;

			// if dp or dl are negative
			// then line crosses.
			if (dp < 0)
			{
				crosses = true;
			}
			// if intersect,
			// then one vertex crosses and no intersection,
			point intersect;
			if (intersection({p, last}, {pp, pl}, intersect))
			{
				// do some clipping here
			}
			allcrosses = allcrosses && crosses;
			last = p;
		}

		return allcrosses; // vertices_.size() == 0;
	}

protected:
    std::vector<point> vertices_;
};

}

#endif /* __GLIB_MODEL__ */