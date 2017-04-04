//
// Created by Mingkai Chen on 2017-03-31.
//

#include "lib/include/camera.h"

#ifdef LIBGRAFIKS_CAMERA_HPP

namespace glib
{

camera::camera (double end, std::pair<double,double> xyscale, point center)
{
	double scaler = std::min(xyscale.first, xyscale.second);
	to_window_ = translate(center).matmul(
        scale(point(scaler /DEFAULT_SCREEN_DIM,
			-scaler / DEFAULT_SCREEN_DIM, 1)));
	planes_ =
	{
		plane({-100, 0, 0}, {1, 0, 0}), // left
		plane({100, 0, 0}, {-1, 0, 0}), // right
		plane({0, -100, 0}, {0, 1, 0}), // down
		plane({0, 100, 0}, {0, -1, 0}), // up
		plane({0, 0, 0}, {0, 0, 1}), // front
		plane({0, 0, end}, {0, 0, -1}) // back
	};
}

void camera::render (shape_render& rend) const
{
	if (!rend.model_->clip_planes(planes_))
	{
		rend.model_->transform(to_window_);
		rend.run({&to_window_});
	}
}

projective_cam::projective_cam (
	double left, double right,
	double up, double down,
	double front, double back,
	std::pair<double,double> xyscale,
	point center) :
Ktrans_(camera_transform(1))
{
	// account for dimensionality
	double camwidth = right-left;
	double camheight = up-down;
	double scaler = std::min(xyscale.first/camwidth, xyscale.second/camheight);

	double centerx = left + camwidth / 2;
	double centery = down + camheight / 2;
	translate camcorrect({-centerx, -centery, 0});
	scale toscreen({2*scaler, -2*scaler, 1});
	to_window_ = translate(center).matmul(toscreen.matmul(camcorrect));

	std::function<double(double)> dist = [](double val)
	{
		return sqrt(1 + val*val);
	};
	planes_ = {
		plane({0, 0, 0}, {1/dist(left), 0, std::abs(left)/dist(left)}),
		plane({0, 0, 0}, {-1/dist(right), 0, std::abs(right)/dist(right)}),
		plane({0, 0, 0}, {0, 1/dist(down), std::abs(down)/dist(down)}),
		plane({0, 0, 0}, {0, -1/dist(up), std::abs(up)/dist(up)}),
		plane({0, 0, front}, {0, 0, 1}),
		plane({0, 0, back}, {0, 0, -1})
	};
}

void projective_cam::render (shape_render& rend) const
{
	if (!rend.model_->clip_planes(planes_))
	{
		// move world to camera
		rend.model_->transform(world2cam_);
		rend.model_->transform(Ktrans_);
		rend.model_->transform(to_window_);
		rend.run({&to_window_, &world2cam_}, &Ktrans_);
	}
}

void projective_cam::transform (const transformation& tr)
{
	for (plane& p : planes_)
	{
		tr.mul(p.pt_.x, p.pt_.y, p.pt_.z);
		tr.mul(p.norm_.x, p.norm_.y, p.norm_.z);
		p.norm_ = p.norm_ - p.pt_;
	}
	CTMP_ = tr.matmul(CTMP_);
	world2cam_ = CTMP_.inverse();
}

}

#endif
