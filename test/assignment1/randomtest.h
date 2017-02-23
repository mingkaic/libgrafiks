#include "test/panel.h"

#ifndef RANDOMTEST
#define RANDOMTEST

struct random_test : public itester
{
    random_test (void) {}
    random_test (size_t nlines) : nlines_(nlines) {}

    virtual void exec (std::vector<glib::ishaper*>& shapers, const glib::point& topcorner, size_t, size_t)
    {
        double x = topcorner.x;
        double y = topcorner.y;
        for (size_t i = 0; i < nlines_; i++)
        {
            unsigned int basecolor = std::rand() % 0x00f0f0f0;
            basecolor += 0xff0f0f0f0f; // base color must be above 0x0f per color channel
            size_t x1 = std::rand() % 300;
            size_t y1 = std::rand() % 300;
            size_t x2 = std::rand() % 300;
            size_t y2 = std::rand() % 300;

			glib::point p1(x+x1, y+y1);
			glib::point p2(x+x2, y+y2);
			p1.basecolor = basecolor;
			p2.basecolor = basecolor;

            glib::line_model line(p1, p2);
            shapers[i % shapers.size()]->draw(&line);
        }
    }

private:
    size_t nlines_ = 30;
};

#endif /* RANDOMTEST */
