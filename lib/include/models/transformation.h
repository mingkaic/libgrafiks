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

	void mul (double& x, double& y, double& z) const;

	// treat as homogenous
	void mul (const double in[3], double out[3]) const;

	// for homogenous coordinates
	// out = this • in
	void h_mul (const double in[4], double out[4]) const;
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
