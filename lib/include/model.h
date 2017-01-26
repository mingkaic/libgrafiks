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

class imodel
{
public:
    unsigned int color_ = 0xffffffff;

    void set_v (POINT pt)
    {
        vertices_.push_back(pt);
    }

    POINT get_v (size_t idx) const
    {
        return vertices_[idx];
    }

protected:
    std::vector<POINT> vertices_;
};

}

#endif /* __GLIB_MODEL__ */