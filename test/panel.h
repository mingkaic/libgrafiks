#include "lib/include/gutils.h"
#include "lib/include/shaper.h"

#ifndef MTESTPANEL
#define MTESTPANEL

struct itester
{
	virtual void exec (std::vector<glib::ishaper*>& shapers, const glib::point& topcorner, size_t width, size_t height) = 0;
};

class panel
{
public:
	panel (glib::point corner1, glib::point corner2) :
		topleft_(corner1),
		botright_(corner2)
	{
		glib::inorder(topleft_, botright_);
	}

	void test (itester& t, std::vector<glib::ishaper*> liner) const
	{
		t.exec(liner, topleft_,
			(size_t) (botright_.getX() - topleft_.getX()),
			(size_t) (botright_.getY() - topleft_.getY()));
	}

	size_t get_width (void) const
	{
		return (size_t) (botright_.getX() - topleft_.getX());
	}

	size_t get_height (void) const
	{
		return (size_t) (botright_.getY() - topleft_.getY());
	}

	std::pair<glib::point, glib::point> get_pts (void) const { return std::pair<glib::point, glib::point>(topleft_, botright_); }

private:
	glib::point topleft_;
	glib::point botright_;
};

#endif /* MTESTPANEL */
