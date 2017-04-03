//
// Created by Mingkai Chen on 2017-03-31.
//

#include "gutils.h"
#include "shaper.h"
#include "models/transformation.h"

#define DEFAULT_SCREEN_DIM 100

#ifndef LIBGRAFIKS_CAMERA_HPP
#define LIBGRAFIKS_CAMERA_HPP

namespace glib
{

class camera
{
public:
	camera (void) {}
	// create an orthogonal camera space with far plane at z coordinate end
	camera (double end, std::pair<double,double> xyscale, point center);

	virtual void render (shape_render& rend) const;

protected:
	transformation to_window_; // projective to screen
	std::vector<plane> planes_;
};

class projective_cam : public camera
{
public:
	projective_cam (
		double left, double right,
		double up, double down,
		double front, double back,
		std::pair<double,double> xyscale,
		point center);

	virtual void render (shape_render& rend) const;

	void transform (const transformation& tr);

private:
	transformation CTMP_; // bring camera to world
	transformation world2cam_; // bring world to camera
	camera_transform Ktrans_; // camera to projective
};

}

#endif //LIBGRAFIKS_CAMERA_HPP
