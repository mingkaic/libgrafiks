// 
// polygon.h
//
// purpose: draws polygons
//

#include "shaper.h"

#ifndef __GLIB_POLYGON__
#define __GLIB_POLYGON__

namespace glib
{

// vertex ids are indices in vertices_
struct poly_model : public shape_model
{
    poly_model (void);
    poly_model (unsigned int color);
    poly_model (std::vector<point> vertices);
    poly_model (std::vector<point> vertices, unsigned int color);

    void set_v (point pt)
    {
        vertices_.push_back(pt);
    }

    // return a heap of indices of vertices sorted by y coordinate
    std::vector<point> ysortindices (void) const;

    // determine if vertices are order counter clockwise relative to its index
    bool cclockwise (void) const;

    bool isconvex (void) const;
};

struct ipolygoner : public ishaper
{
    ipolygoner (DRAW drawable);

    virtual void draw (const shape_model* model) const;
    
    virtual void draw (const poly_model* model) const = 0;
};

}

#endif /* __GLIB_POLYGON__ */