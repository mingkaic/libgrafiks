//
// Created by Mingkai Chen on 2017-03-29.
//

#include <cstdlib>
#include <cstdint>

#ifndef LIBGRAFIKS_COLOR_HPP
#define LIBGRAFIKS_COLOR_HPP

namespace glib
{

struct color
{
	color (unsigned base);

	color (uint8_t r, uint8_t g, uint8_t b);

	explicit operator unsigned() const;

	uint8_t r;
	uint8_t g;
	uint8_t b;
};

struct color_grad
{
	color_grad (unsigned base);

	color_grad (color c);

	color_grad (double r, double g, double b);

	operator unsigned() const;

	const color_grad& operator += (const color_grad& other);

	double r;
	double g;
	double b;
};

color_grad operator - (const color& lhs, const color& rhs);

color_grad operator / (const color_grad& lhs, double rhs);

color_grad operator * (const color_grad& lhs, double magnitude);

color operator * (const color& lhs, double magnitude);

}

#endif //LIBGRAFIKS_COLOR_HPP
