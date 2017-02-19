// 
// model.h
//
// purpose: provide an interface to modeling polygons
//

#include <vector>
#include "gutils.h"

#ifndef __GLIB_MODEL__
#define __GLIB_MODEL__

namespace glib
{

class shape_model
{
public:
    unsigned int color_ = 0xffffffff;

	point get_v (size_t idx) const
	{
		return vertices_[idx];
	}

protected:
    std::vector<point> vertices_;
};

}

#endif /* __GLIB_MODEL__ */