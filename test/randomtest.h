#include "panel.h"

#ifndef RANDOMTEST
#define RANDOMTEST

namespace mingtest
{

struct random_test : public itester
{
    random_test (void) {}
    random_test (size_t nlines) : nlines_(nlines) {}

    virtual void exec (std::vector<glib::ishaper*>& shapers, POINT& topcorner, size_t width, size_t height)
    {
        double x = topcorner.first;
        double y = topcorner.second;
        for (size_t i = 0; i < nlines_; i++)
        {
            unsigned int basecolor = std::rand() % 0x00f0f0f0;
            basecolor += 0xff0f0f0f0f; // base color must be above 0x0f per color channel
            size_t x1 = std::rand() % 300;
            size_t y1 = std::rand() % 300;
            size_t x2 = std::rand() % 300;
            size_t y2 = std::rand() % 300;
            glib::line_model line(POINT(x+x1, y+y1), POINT(x+x2, y+y2), basecolor);
            shapers[i % shapers.size()]->draw(&line);
        }
    }

private:
    size_t nlines_ = 30;
};

}

#endif /* RANDOMTEST */