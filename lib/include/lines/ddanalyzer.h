// 
// ddanalyzer.h
//
// purpose: draws lines using dda
//

#include "line.h"

#ifndef __GLIB_DDA__
#define __GLIB_DDA__

namespace glib
{

struct dda_liner : public iliner
{
	dda_liner (DRAW draw);

	virtual void draw (const line_model* model) const;
};

}

#endif /* __GLIB_DDA__ */