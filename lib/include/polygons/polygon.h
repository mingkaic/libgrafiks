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
struct poly_model : public imodel
{
    poly_model (void);
    poly_model (unsigned int color);
    poly_model (std::vector<POINT> vertices);
    poly_model (std::vector<POINT> vertices, unsigned int color);

    // return a heap of indices of vertices sorted by y coordinate
    std::vector<POINT> ysortindices (void) const;

    // determine if vertices are order counter clockwise relative to its index
    bool cclockwise (void) const;
};

struct ipolygoner : public ishaper
{
    ipolygoner (DRAW drawable);

    virtual void draw (const imodel* model) const;
    
    virtual void draw (const poly_model* model) const = 0;
};

}

#endif /* __GLIB_POLYGON__ */