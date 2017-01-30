//
// coord.h
//
// purpose: model and transform coordinates
//

#ifndef __GLIB_COORD__
#define __GLIB_COORD__

namespace glib
{

// homogenous coordinate
struct hcoord
{
	coord (std::vector<double> dval)
	{
		// find common denom in dval
	}

	double& operator [] (size_t idx)
	{
		if (idx == cartesian_.size())
		{
			return projective_;
		}
		return (double)cartesian_[idx] / projective_;
	}

	std::vector<size_t> cartesian_;
	double projective_;
};

}

#endif /* __GLIB_COORD__ */