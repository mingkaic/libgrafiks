//
// Created by Mingkai Chen on 2017-03-29.
//

#include <vector>

#include "lib/include/gutils.h"
#include "lib/include/light/color.h"

#ifndef LIBGRAFIKS_LIGHT_HPP
#define LIBGRAFIKS_LIGHT_HPP

namespace glib
{

struct light
{
	struct bulb
	{
		double A;
		double B;
		normal p_;
		color_grad srcI;
	};

	color operator () (double x, double y, double z,
		const color& basecolor, const normal& norm) const
	{
		// assert that x, y, z is a point wrt to camera at origin
		// that is everything is transformed to camera coordinates
		color c = basecolor;
		c.r *= ambient_.r;
		c.g *= ambient_.g;
		c.b *= ambient_.b;

		for (bulb b : srcs_)
		{
			double dx = b.p_.x - x;
			double dy = b.p_.y - y;
			double dz = b.p_.z - z;
			double dist = sqrt(dx*dx + dy*dy + dz*dz);
			double fatti = 1 / (b.A + b.B*dist);


 		}

		return c;
	}

	color_grad ambient_ = 0;
	std::vector<bulb> srcs_;
};

}

#endif //LIBGRAFIKS_LIGHT_HPP
