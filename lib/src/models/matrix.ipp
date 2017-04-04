//
// Created by Mingkai Chen on 2017-02-17.
//

#ifdef LIBGRAFIKS_MATRIX_HPP

namespace glib
{

template <size_t N>
sqr_mat<N> sqr_mat<N>::matmul (const sqr_mat<N>& other) const
{
	double res[N][N];
	for (size_t y = 0; y < N; y++)
	{
		for (size_t x = 0; x < N; x++)
		{
			res[y][x] = 0;
			for (size_t i = 0; i < N; i++)
			{
				res[y][x] += data_[y][i] * other.data_[i][x];
			}
		}
	}
	return sqr_mat<N>(res);
}

template <size_t N>
sqr_mat<N> sqr_mat<N>::inverse (void) const
{
	double det = determinant(data_);
	if (0 == det)
	{
		throw std::exception();
	}
	double res[N][N];
	adjoint(data_, res);
	for (size_t y = 0; y < N; y++)
	{
		for (size_t x = 0; x < N; x++)
		{
			res[y][x] /= det;
		}
	}
	return sqr_mat<N>(res);
}

template <size_t N>
sqr_mat<N> sqr_mat<N>::transpose (void) const
{
	double res[N][N];
	std::memcpy(res, data_, sizeof(double)*N*N);
	for (size_t y = 1; y < N; y++)
	{
		for (size_t x = 0; x < y; x++)
		{
			std::swap(res[y][x], res[x][y]);
		}
	}
	return sqr_mat<N>(res);
}

template <size_t N>
void sqr_mat<N>::cofactor(const double in[N][N], double out[N][N],
	size_t r, size_t c, size_t n) const
{
	int i = 0, j = 0;
	for (size_t y = 0; y < n; y++)
	{
		if (y != r)
		{
			for (int x = 0; x < n; x++)
			{
				if (x != c)
				{
					out[i][j] = in[y][x];
					j++;
				}
			}
			j = 0;
			i++;
		}
	}
}

template <size_t N>
double sqr_mat<N>::determinant (const double in[N][N], size_t n) const
{
	if (n == 1) return in[0][0];

	double det = 0;
	double cof[N][N];
	int sign = 1;
	for (int x = 0; x < n; x++)
	{
		sqr_mat<N>::cofactor(in, cof, 0, x, n);
		det += sign * in[0][x] * determinant(cof, n-1);
		sign = -sign;
	}
	return det;
}

template <size_t N>
void sqr_mat<N>::adjoint (const double in[N][N], double out[N][N]) const
{
	if (N == 1) { out[0][0] = 1; return; }
		double cof[N][N];
	for (size_t y = 0; y < N; y++)
	{
		for (size_t x = 0; x < N; x++)
		{
			cofactor(in, cof, y, x, N);

			out[x][y] = determinant(cof, N-1);
			if ((x + y) % 2 != 0)
			{
				out[x][y] = -out[x][y];
			}
		}
	}
}

}

#endif
