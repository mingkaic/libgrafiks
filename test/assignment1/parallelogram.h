#include "test/panel.h"
#include "lib/include/lines/line.h"

#ifndef PARALLELOTEST
#define PARALLELOTEST

struct paralleogram_test : public itester
{
	paralleogram_test (void) {}
	paralleogram_test (size_t nparalells) : nparallel_(nparalells) {}

    virtual void exec (std::vector<glib::ishaper*>& shapers, const glib::point& topcorner, size_t, size_t)
	{
		double x = topcorner.getX();
		double y = topcorner.getY();
		for (size_t i = 0; i < nparallel_; i++)
		{
			glib::line_model line0(glib::point(x+20, y+80+i), glib::point(x+150, y+150+i));
			glib::line_model line1(glib::point(x+160+i, y+270), glib::point(x+240+i, y+40));
			shapers[i % shapers.size()]->draw(&line0);
			shapers[i % shapers.size()]->draw(&line1);
		}
	}

private:
	size_t nparallel_ = 50;
};

#endif /* PARALLELOTEST */
