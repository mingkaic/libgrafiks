// 
// shaper.h
//
// purpose: interface for drawing shapes
//

#include <cassert>
#include <cmath>
#include <memory>

#include "models/model.h"
#include "gutils.h"

#ifndef __GLIB_SHAPE__
#define __GLIB_SHAPE__

namespace glib
{

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
	std::shared_ptr<ishaper> shaper_ = nullptr;
	shape_model* model_ = nullptr;

	void run (void) {
		assert(shaper_ && model_);
		shaper_->draw(model_);
	}
};

}

#endif /* __GLIB_SHAPE__ */
