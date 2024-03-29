//
// Created by Mingkai Chen on 2017-03-31.
//

#include "lib/include/models/model.h"

#ifdef __GLIB_MODEL__

namespace glib
{

point shape_model::get_v (size_t idx) const
{
	return vertices_[idx];
}

void shape_model::transform (const transformation& trans)
{
	for (point& p : vertices_)
	{
		trans.mul(p.x, p.y, p.z, p.h);
	}
}

size_t shape_model::n_vertices (void) const
{
	return vertices_.size();
}

void shape_model::add_point (const std::vector<point>& pts)
{
	size_t n = pts.size();
	assert(n <= vertices_.size());
	for (size_t i = 0;
		 i < n; i++)
	{
		vertices_[i].x += pts[i].x;
		vertices_[i].y += pts[i].y;
		vertices_[i].z += pts[i].z;
		vertices_[i].h += pts[i].h;
	}
}

bool shape_model::clip_planes (const std::vector<plane>& planes)
{
	// find all edges
	for (const plane& p : planes)
	{
		const normal& norm = p.norm_;
		std::unordered_map<point*,bool> vstatus;
		for (point& pt : vertices_)
		{
			normal pn = normal{
				pt.getX(),
				pt.getY(),
				pt.getZ()
			};
			normal tpt = pn - p.pt_;
			double dotres = dot(tpt, norm);
			vstatus[&pt] = 0 <= dotres;
		}

		std::vector<point> resv;
		point* startv = &vertices_.back();
		for (point& endv : vertices_)
		{
			bool evalid = vstatus[&endv];
			if (vstatus[startv] != evalid)
			{
				point ipt;
				if (intersect(p, *startv, endv, ipt))
				{
					resv.push_back(ipt);
				}
			}

			if (evalid)
			{
				resv.push_back(endv);
			}
			startv = &endv;
		}
		vertices_ = resv;
	}

	return vertices_.empty();
}

bool shape_model::intersect (
	const plane& separator,
	const point& p1,
	const point& p2,
	point& respt)
{
	normal L = {
		p2.getX() - p1.getX(),
		p2.getY() - p1.getY(),
		p2.getZ() - p1.getZ()
	};
	double totaldist = dist(L);
	L.x /= totaldist;
	L.y /= totaldist;
	L.z /= totaldist;
	const normal& norm = separator.norm_;

	double denom = dot(L, norm);
	// there is no intersection if denom == 0
	if (0 == denom)
	{
		return false;
	}

	// get distance from p1 to plane
	// d = dot((pt - pn1), norm) / dot(L, norm)
	const normal& pt = separator.pt_;
	normal pn1(p1.getX(), p1.getY(), p1.getZ());
	double p1dist = dot(pt - pn1, norm) / denom;
	color_grad cd = p2.basecolor - p1.basecolor;
	normal nd = p2.n - p1.n;

	// resulting point is either p1 or p2, no point in adding more points
	if (0 == p1dist || totaldist == p1dist)
	{
		return false;
	}

	// build resulting point
	respt = {
		p1.getX() + L.x * p1dist,
		p1.getY() + L.y * p1dist,
		p1.getZ() + L.z * p1dist
	};
	color_grad cg = p1.basecolor;
	normal ng = p1.n;
	cg += cd * p1dist / totaldist;
	ng = ng + p1dist / totaldist * nd;
	respt.basecolor = (unsigned)cg;
	respt.n = ng;

	return true;
}

}

#endif
