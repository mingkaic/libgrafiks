#include "test/panel.h"
#include "test/assignment1/common.h"
#include "lib/include/lines/line.h"

#ifndef STARBURSTTEST
#define STARBURSTTEST

struct starburst_test : public itester
{
	starburst_test (void) {}
	starburst_test (size_t length, size_t nlines) :
		lineLength_(length), nlines_(nlines) {}

	virtual void exec (std::vector<glib::ishaper*>& shapers, const glib::point& topcorner, size_t width, size_t height)
	{
		assert(width >= lineLength_ * 2 && height >= lineLength_ * 2); // length requirement
        size_t centerx = (size_t)(topcorner.getX() + width/2);
        size_t centery = (size_t)(topcorner.getY() + height/2);
        glib::point origin((double)centerx, (double)centery);
		std::vector<glib::point> pts = starburst_vertices(centerx, centery, lineLength_, nlines_);
		size_t i = 0;
		for (glib::point p : pts)
		{
			glib::line_model line(origin, p);
			shapers[i % shapers.size()]->draw(&line);
			i++;
		}
	}

private:
	size_t lineLength_ = 125;
	size_t nlines_ = 90;
};

#endif /* STARBURSTTEST */
