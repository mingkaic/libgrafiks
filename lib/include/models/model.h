// 
// model.h
//
// purpose: provide an interface to modeling polygons
//

#include <vector>
#include <unordered_map>

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
			trans.mul(p.x, p.y, p.z, p.h);
		}
	}

	void colorpoints (std::function<void(color&)> ctrans)
	{
		for (point& v : vertices_)
		{
			ctrans(v.basecolor);
		}
	}

	size_t n_vertices (void) const
	{
		return vertices_.size();
	}

	void add_point (const std::vector<point>& pts)
	{
		size_t n = pts.size();
		assert(n <= vertices_.size());
		for (size_t i = 0;
			i < n; i++)
		{
			vertices_[i].x = pts[i].x + vertices_[i].x;
			vertices_[i].y = pts[i].y + vertices_[i].y;
			vertices_[i].z = pts[i].z + vertices_[i].z;
			vertices_[i].h = pts[i].h + vertices_[i].h;
		}
	}

	// normal direction points inward
	// (normal must be unit)
	// vertices outside the designated direction "crosses" the plane
	// return true if model is culled
	bool clip_planes (const std::vector<plane>& planes)
	{
		size_t nv = vertices_.size();
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
				double dotres =
					tpt.x * norm.x +
					tpt.y * norm.y +
					tpt.z * norm.z;
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

protected:
	bool intersect (
		const plane& separator,
		const point& p1,
		const point& p2,
		point& respt)
	{
		auto dist = [](normal& n)
		{
			return sqrt(n.x * n.x + n.y * n.y + n.z * n.z);
		};
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
		color_grad cd = p2.basecolor - p1.basecolor;;

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
		cg += cd * p1dist / totaldist;
		respt.basecolor = (unsigned)cg;

		return true;
	}

    std::vector<point> vertices_;
};

}

#endif /* __GLIB_MODEL__ */