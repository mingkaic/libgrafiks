//
// Created by Mingkai Chen on 2017-03-31.
//

#include "lib/include/camera.h"

#ifdef LIBGRAFIKS_CAMERA_HPP

namespace glib
{

camera::camera (double end, std::pair<double,double> screenWH, point center)
{
	to_window_ = translate(center).matmul(
		scale(point(screenWH.first  /(2*DEFAULT_SCREEN_DIM),
			-screenWH.second/(2*DEFAULT_SCREEN_DIM), 1)));
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
		rend.run();
	}
}

projective_cam::projective_cam (
	double left, double right,
	double up, double down,
	double front, double back,
	std::pair<double,double> screenWH,
	point center) :
Ktrans_(camera_transform(1))
{
	to_window_ = translate(center).matmul(
		scale(point{
			2*screenWH.first/(right-left),
			-2*screenWH.second/(up-down), 1}));
	auto dist = [](double val)
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
		// cache model's z coord
		std::vector<point> zs;
		for (size_t i = 0, n = rend.model_->n_vertices();
			 i < n; i++)
		{
			point p = rend.model_->get_v(i);
			zs.push_back({0, 0, p.z * p.z, 0});
		}
		rend.model_->transform(Ktrans_);
		// add z coord to z
		rend.model_->add_point(zs);
		rend.model_->transform(to_window_);
		rend.run();
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
	CTMP_ = CTMP_.matmul(tr);
	world2cam_ = CTMP_.inverse();
}

}

#endif