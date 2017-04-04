//
// Created by Mingkai Chen on 2017-02-18.
//

#include <cmath>
#include <cassert>
#include <vector>

#include "lib/include/gutils.h"
#include "lib/include/models/matrix.h"

#ifndef LIBGRAFIKS_TRANSFORMATION_HPP
#define LIBGRAFIKS_TRANSFORMATION_HPP

namespace glib
{

class transformation : public sqr_mat<4> {
public:
	transformation (void);
	transformation (double data[4][4]) :
		sqr_mat<4>(data) {}
	transformation (const sqr_mat<4>& other) :
		sqr_mat<4>(other) {}

	virtual void mul (double& x, double& y, double& z) const;

	virtual void mul (double& x, double& y, double& z, double& h) const;

	// treat as homogenous
	void mul (const double in[3], double out[3]) const;

	// for homogenous coordinates
	// out = this • in
	void h_mul (const double in[4], double out[4]) const;
};

// no matmuling, only mul
class camera_transform : public transformation
{
public:
	camera_transform (double projectz);

	virtual void mul (double& x, double& y, double& z) const
	{
		double zz = z;
		double h = 1;
		transformation::mul(x, y, z, h);
		x /= h;
		y /= h;
		z = zz;
	}

	virtual void mul (double& x, double& y, double& z, double& h) const
	{
		double zz = z;
		transformation::mul(x, y, z, h);
		x /= std::abs(h);
		y /= std::abs(h);
		h = 1;
		z = zz;
	}

	void rmul (double& x, double& y, double& z, double&) const
	{
		x *= std::abs(z) / projectz;
		y *= std::abs(z) / projectz;
	}

	virtual sqr_mat<4> matmul (const sqr_mat<4>& other) const { return other; }

	virtual sqr_mat<4> inverse (void) const { return *this; }

private:
	double projectz;
};

class scale : public transformation
{
public:
	scale (point pt);
};

enum THETA_UNIT
{
	DEGREE,
	RADIAN
};

class rotate : public transformation
{
public:
	rotate (char axis, double degree,
		THETA_UNIT unit = DEGREE);
};

class translate : public transformation
{
public:
	translate (point pt);
};

}

#endif //LIBGRAFIKS_TRANSFORMATION_HPP
