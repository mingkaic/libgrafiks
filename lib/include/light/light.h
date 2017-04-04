//
// Created by Mingkai Chen on 2017-03-29.
//

#include <vector>

#include "lib/include/gutils.h"
#include "lib/include/light/color.h"
#include "lib/include/models/transformation.h"

#ifndef LIBGRAFIKS_LIGHT_HPP
#define LIBGRAFIKS_LIGHT_HPP

namespace glib
{

struct light
{
static double super;
	struct bulb
	{
		bulb (double A, double B, normal p, color_grad si) :
			A(A), B(B), p_(p), srcI(si) {}

		double A;
		double B;
		normal p_;
		color_grad srcI;
	};

	color operator () (
		normal surface, const normal& N,
		std::vector<const transformation*> trans, const camera_transform* Ktrans,
		const color_grad& kd, double ks, double p) const
	{
		assert(trans.empty() == false);
		// reverse translate surface
		double h = 1;
		transformation backer = trans[0]->inverse();
		backer.mul(surface.x, surface.y, surface.z, h);
		if (Ktrans)
		{
			Ktrans->rmul(surface.x, surface.y, surface.z, h);
		}
		if (trans.size() > 1)
		{
			transformation worlder = trans[1]->inverse();
			worlder.mul(surface.x, surface.y, surface.z, h);
		}
		surface.x /= h;
		surface.y /= h;
		surface.z /= h;

		normal V = -1 * surface;
		V.normalize();
		// assert that x, y, z is a point wrt to camera at origin
		// that is everything is transformed to camera coordinates
		double r = kd.r * ambient_.r;
		double g = kd.g * ambient_.g;
		double b = kd.b * ambient_.b;

		for (std::shared_ptr<bulb> bu : srcs_)
		{
			normal L = bu->p_ - surface;
			double dl = dist(L);
			double fatti = 1 / (bu->A + bu->B*dl);
if (std::abs(super - fatti) > 0.05 && super != 0)
{
	std::cout << "stop\n";
}
super = fatti;
			L.normalize();

			double diff = dot(L, N);
			if (diff < 0) continue; // diff < 0 means light is behind surface

			normal R = 2 * dot(N, L) * N - L;
			R.normalize();
			double spec = ks * std::pow(dot(R, V), p);

			r += bu->srcI.r * fatti * (kd.r * diff + spec);
			g += bu->srcI.g * fatti * (kd.g * diff + spec);
			b += bu->srcI.b * fatti * (kd.b * diff + spec);
 		}

 		// clip the values to prevent overflow
 		r = std::min(255.0, 255*r);
		g = std::min(255.0, 255*g);
		b = std::min(255.0, 255*b);

		return color(r, g, b);
	}

	color_grad ambient_ = 0;
	std::vector<std::shared_ptr<bulb> > srcs_;
};

}

#endif //LIBGRAFIKS_LIGHT_HPP
