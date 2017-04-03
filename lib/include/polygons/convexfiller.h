// 
// convexfiller.h
//
// purpose: draws polygon using convex filling algorithm
//

#include "lib/include/polygons/polygon.h"

#ifndef __GLIB_CONVEXFILL__
#define __GLIB_CONVEXFILL__

namespace glib
{

struct convex_filler : public ipolygoner
{
	convex_filler (DRAW draw);

	virtual void draw (const poly_model* model) const;
};

}

#endif /* __GLIB_CONVEXFILL__ */
