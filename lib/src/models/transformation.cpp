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

void transformation::mul (double& x, double& y, double& z, double& h) const
{
	double in[4] = {x, y, z, h};
	double res[4];
	h_mul(in, res);
	x = res[0];
	y = res[1];
	z = res[2];
	h = res[3];
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

camera_transform::camera_transform (double near)
{
	data_[3][2] = 1/near;
	data_[2][2] = data_[3][3] = 0;
}

scale::scale (point pt)
{
	data_[0][0] = pt.getX();
	data_[1][1] = pt.getY();
	data_[2][2] = pt.getZ();
}

rotate::rotate (char axis, double theta, THETA_UNIT unit)
{
	size_t idx = axis - 'X';
	assert(idx < 3);
	if (unit == DEGREE)
	{
		theta = M_PI * theta / 180;
	}
	double c = std::cos(theta);
	double s = std::sin(theta);
	std::vector<size_t> cofactors = {0, 1, 2};
	cofactors.erase(cofactors.begin() + idx);
	size_t c1 = cofactors[0];
	size_t c2 = cofactors[1];
	data_[c1][c1] = data_[c2][c2] = c; // cofactor diagonals are cos
	data_[c1][c2] = data_[c2][c1] = s;
	if (axis == 'Y')
	{
		data_[c1][c2] = -data_[c1][c2];
	}
	else
	{
		data_[c2][c1] = -data_[c2][c1]; // bottom left is negative for Y only
	}
}

translate::translate (point pt)
{
	data_[0][3] = pt.getX();
	data_[1][3] = pt.getY();
	data_[2][3] = pt.getZ();
}

}

#endif