//
// Created by Mingkai Chen on 2017-03-29.
//

#include "lib/include/light/color.h"

#ifdef LIBGRAFIKS_COLOR_HPP

namespace glib
{

color::color (unsigned base)
{
	r = (base & 0x00ff0000) >> 16;
	g = (base & 0x0000ff00) >> 8;
	b = base & 0x000000ff;
}

color::color (uint8_t r, uint8_t g, uint8_t b) :
	r(r), g(g), b(b) {}

color::operator unsigned() const
{
	return (0xff << 24) + (r << 16) + (g << 8) + b;
}

color_grad::color_grad (unsigned base)
{
	r = (double) ((base & 0x00ff0000) >> 16);
	g = (double) ((base & 0x0000ff00) >> 8);
	b = (double) (base & 0x000000ff);
}

color_grad::color_grad (color c)
{
	r = (double) c.r;
	g = (double) c.g;
	b = (double) c.b;
}

color_grad::color_grad (double r, double g, double b) :
	r(r), g(g), b(b) {}

color_grad::operator unsigned() const
{
	uint8_t red = (uint8_t) r;
	uint8_t green = (uint8_t) g;
	uint8_t blue = (uint8_t) b;
	return (0xff << 24) + (red << 16) + (green << 8) + blue;
}

const color_grad& color_grad::operator += (const color_grad& other)
{
	r += other.r;
	g += other.g;
	b += other.b;
	return *this;
}

color_grad operator - (const color& lhs, const color& rhs)
{
	return color_grad{
			(double) lhs.r - (double) rhs.r,
			(double) lhs.g - (double) rhs.g,
			(double) lhs.b - (double) rhs.b
	};
}

color_grad operator / (const color_grad& lhs, double rhs)
{
	return color_grad{
			lhs.r / rhs,
			lhs.g / rhs,
			lhs.b / rhs
	};
}

color_grad operator * (const color_grad& lhs, double magnitude)
{
	return color_grad{
			lhs.r * magnitude,
			lhs.g * magnitude,
			lhs.b * magnitude
	};
}

color operator * (const color& lhs, double magnitude)
{
	return color{
			(uint8_t) (lhs.r * magnitude),
			(uint8_t) (lhs.g * magnitude),
			(uint8_t) (lhs.b * magnitude)
	};
}

}

#endif