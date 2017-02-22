// 
// line.h
//
// purpose: draws lines
//

#include "shaper.h"
#include "models/transformation.h"

#ifndef __GLIB_LINE__
#define __GLIB_LINE__

namespace glib
{

struct line_model : public shape_model
{
    line_model (point a, point b);

    double dx (void) const;

    double dy (void) const;

    double dz (void) const;

	color_grad dc (void) const;
};

class iliner : public ishaper
{
public:
    iliner (DRAW drawable);

    virtual void draw (const shape_model* model) const;
    
    virtual void draw (const line_model* model) const = 0;

protected:
    // returns the degree of rotation to ensure octant 1 alignment
    const transformation& octantize (const line_model& model) const;
};

}

#endif /* __GLIB_LINE__ */