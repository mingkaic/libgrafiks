// 
// line.h
//
// purpose: draws lines
//

#include "shaper.h"

#ifndef __GLIB_LINE__
#define __GLIB_LINE__

namespace glib
{

// coordinate transformation function
#define COORD_TRANS std::function<void(double&,double&)>

struct line_model : public imodel
{
    line_model (POINT a, POINT b);
    line_model (POINT a, POINT b, unsigned int color);

    double dx (void) const;

    double dy (void) const;
};

class iliner : public ishaper
{
public:
    iliner (DRAW drawable);

    virtual void draw (const imodel* model) const;
    
    virtual void draw (const line_model* model) const = 0;

protected:
    // returns a commutative function mapping octant 1 coordinate to real coordinate
    const coord_transform& octantize (const line_model& model) const;
};

}

#endif /* __GLIB_LINE__ */