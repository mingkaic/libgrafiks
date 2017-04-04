// 
// model.h
//
// purpose: provide an interface to modeling polygons
//

#include <vector>
#include <unordered_map>

#include "lib/include/gutils.h"
#include "lib/include/models/transformation.h"

#ifndef __GLIB_MODEL__
#define __GLIB_MODEL__

namespace glib
{

using LIGHTCOLOR = std::function<unsigned(double,double,double,color_grad,normal)>;

class shape_model
{
public:
	virtual ~shape_model (void) {}

	point get_v (size_t idx) const;

	void transform (const transformation& trans);

	size_t n_vertices (void) const;

	void add_point (const std::vector<point>& pts);

	// normal direction points inward
	// (normal must be unit)
	// vertices outside the designated direction "crosses" the plane
	// return true if model is culled
	bool clip_planes (const std::vector<plane>& planes);

	color* face_color_ = nullptr;
	LIGHTCOLOR shader_;

protected:
	bool intersect (
		const plane& separator,
		const point& p1,
		const point& p2,
		point& respt);

	std::vector<point> vertices_;
};

}

#endif /* __GLIB_MODEL__ */
