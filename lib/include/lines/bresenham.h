// 
// bresenham.h
//
// purpose: draws lines using bresenham's algorithm
//

#include "line.h"

#ifndef __GLIB_BRESEN__
#define __GLIB_BRESEN__

namespace glib
{

struct bresen_liner : public iliner
{
    bresen_liner (DRAW draw);

    virtual void draw (const line_model* model) const;
};

}

#endif /* __GLIB_BRESEN__ */