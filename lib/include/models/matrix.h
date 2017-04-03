//
// Created by Mingkai Chen on 2017-02-17.
//

#include <cstdlib>
#include <cstring>
#include <exception>

#ifndef LIBGRAFIKS_MATRIX_HPP
#define LIBGRAFIKS_MATRIX_HPP

namespace glib
{

template <size_t N>
class sqr_mat
{
public:
	sqr_mat (double data[N][N])
	{
		std::memcpy(data_, data, sizeof(double) * N * N);
	}

	sqr_mat (const sqr_mat<N>& other)
	{
		std::memcpy(data_, other.data_, sizeof(double) * N * N);
	}

	// this * other
	virtual sqr_mat<N> matmul (const sqr_mat<N>& other) const;

	virtual sqr_mat<N> inverse (void) const;

	sqr_mat<N> transpose (void) const;

protected:
	double data_[N][N]; // <y> <x>

private:
	void cofactor(const double in[N][N], double out[N][N],
		size_t r, size_t c, size_t n) const;

	double determinant (const double in[N][N], size_t n = N) const;

	void adjoint (const double in[N][N], double out[N][N]) const;
};

}

#include "../../src/models/matrix.ipp"

#endif //LIBGRAFIKS_MATRIX_HPP
