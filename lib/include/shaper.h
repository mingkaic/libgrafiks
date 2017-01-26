// 
// shaper.h
//
// purpose: interface for drawing shapes
//

#include <cmath>
#include "model.h"
#include "gutils.h"

#ifndef __GLIB_SHAPE__
#define __GLIB_SHAPE__

namespace glib
{

class ishaper
{
public:
    ishaper (DRAW drawable) : drawable_(drawable) {}

    virtual void draw (const imodel* model) const = 0;

protected:
    DRAW drawable_;
};

}

#endif /* __GLIB_SHAPE__ */