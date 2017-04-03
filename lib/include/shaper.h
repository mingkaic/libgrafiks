// 
// shaper.h
//
// purpose: interface for drawing shapes
//

#include <cassert>
#include <cmath>
#include <memory>

#include "gutils.h"
#include "models/model.h"
#include "lib/include/light/light.h"

#ifndef __GLIB_SHAPE__
#define __GLIB_SHAPE__

namespace glib
{

enum SHADING_METHOD
{
	FLAT_SHAD = 0,
	GOURAUD_SHAD,
	PHONG_SHAD
};

class ishaper
{
public:
	ishaper (DRAW drawable) : drawable_(drawable) {}

	virtual void draw (const shape_model* model) const = 0;

protected:
	DRAW drawable_;
};

// rendering wrapper object
struct shape_render
{
	void run (std::vector<const transformation*> trans, const camera_transform* Ktrans = nullptr);

	std::shared_ptr<ishaper> shaper_ = nullptr;
	shape_model* model_ = nullptr;
	light sources_;
	SHADING_METHOD shad_ = FLAT_SHAD;
};

}

#endif /* __GLIB_SHAPE__ */
