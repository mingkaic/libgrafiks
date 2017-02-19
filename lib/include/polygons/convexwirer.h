//
// Created by Mingkai Chen on 2017-02-18.
//

#include "polygons/polygon.h"

#ifndef LIBGRAFIKS_CONVEXWIRER_HPP
#define LIBGRAFIKS_CONVEXWIRER_HPP

namespace glib
{

struct convex_wirer : public ipolygoner
{
	convex_wirer (DRAW draw);

	virtual void draw (const poly_model* model) const;
};

}

#endif //LIBGRAFIKS_CONVEXWIRER_HPP
