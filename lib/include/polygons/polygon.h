// 
// polygon.h
//
// purpose: draws polygons
//

#include "lib/include/shaper.h"

#ifndef __GLIB_POLYGON__
#define __GLIB_POLYGON__

namespace glib
{

// vertex ids are indices in vertices_
struct poly_model : public shape_model
{
	poly_model (void);
	poly_model (std::vector<point> vertices);

	void set_v (point pt)
	{
		vertices_.push_back(pt);
	}

	void lerp_norm (void)
	{
		if (vertices_.size() < 3) return;
		point& A = vertices_[0];
		point& B = vertices_[1];
		point& C = vertices_[2];
		normal mid(B.getX(), B.getY(), B.getZ());

		normal v1(A.getX(), A.getY(), A.getZ());
		normal v2(C.getX(), C.getY(), C.getZ());
		v1 = v1 - mid;
		v2 = v2 - mid;
		// find face norm by cross product of v1 x v2 since we only render counter clockwise polygons
		normal truenorm = cross(v1, v2);

		for (point& p : vertices_)
		{
			double d = dist(p.n);
			if (d == 0)
			{
				p.n = truenorm;
			}
			p.n.normalize();
		}
	}

	// return a heap of indices of vertices sorted by y coordinate
	std::vector<point> ysortindices (void) const;

	// determine if vertices are order counter clockwise relative to its index
	bool cclockwise (void) const;

	bool isconvex (void) const;

	double ks_ = 0.3;
	double p_ = 8;
};

struct ipolygoner : public ishaper
{
	ipolygoner (DRAW drawable);

	virtual void draw (const shape_model* model) const;
	
	virtual void draw (const poly_model* model) const = 0;
};

}

#endif /* __GLIB_POLYGON__ */
