// 
// antialias.h
//
// purpose: draws lines using antialiasing
//

#include "line.h"

#ifndef __GLIB_AALIAS__
#define __GLIB_AALIAS__

namespace glib
{

// let the width of the line be sqrt(2) pixel width
// let the radius of the pixel model be sqrt(2)/2
// antialias using dda
class aalias_liner : public iliner
{
public:
    aalias_liner (DRAW draw);

    // (exactly the center of the square pixel to its edge)
    virtual void draw (const line_model* model) const;

private:
    // input: distance from line center to desired pixel center
    // return the percentage of line model to pixel center
    double intensity (double dlcenter) const;
};

}

#endif /* __GLIB_AALIAS__ */