//
// Created by Mingkai Chen on 2017-02-18.
//

#include "lib/include/models/transformation.h"

#ifdef LIBGRAFIKS_TRANSFORMATION_HPP

namespace glib
{

static double identity[4][4] =
{
	1, 0, 0, 0,
	0, 1, 0, 0,
	0, 0, 1, 0,
	0, 0, 0, 1
};

transformation::transformation (void) :
	sqr_mat<4>(identity) {}

void transformation::mul (double& x, double& y, double& z) const
{
	double in[3] = {x, y, z};
	double res[3];
	mul(in, res);
	x = res[0];
	y = res[1];
	z = res[2];
}

void transformation::mul (const double in[3], double out[3]) const
{
	for (size_t j = 0; j < 3; j++)
	{
		out[j] = 0;
		for (size_t i = 0; i < 3; i++)
		{
			out[j] += this->data_[j][i] * in[i];
		}
		out[j] += this->data_[j][3] * 1;
	}
}

void transformation::h_mul (const double in[4], double out[4]) const
{
	for (size_t j = 0; j < 4; j++)
	{
		out[j] = 0;
		for (size_t i = 0; i < 4; i++)
		{
			out[j] += this->data_[j][i] * in[i];
		}
	}
}

scale::scale (point pt)
{
	data_[0][0] = pt.x;
	data_[1][1] = pt.y;
	data_[2][2] = pt.z;
}

rotate::rotate (char axis, double theta, THETA_UNIT unit)
{
	size_t idx = axis - 'X';
	assert(idx < 3);
	if (unit == DEGREE)
	{
		theta = M_PI * 180 / theta;
	}
	double c = std::cos(theta);
	double s = std::sin(theta);
	std::vector<size_t> cofactors = {0, 1, 2};
	cofactors.erase(cofactors.begin() + idx);
	size_t c1 = cofactors[0];
	size_t c2 = cofactors[1];
	data_[c1][c1] = data_[c2][c2] = c; // cofactor diagonals are cos
	data_[c1][c2] = -s; // cofactor top left is negative sin
	data_[c2][c1] = s;
}

translate::translate (point pt)
{
	data_[0][3] = pt.x;
	data_[1][3] = pt.y;
	data_[2][3] = pt.z;
}

}

#endif