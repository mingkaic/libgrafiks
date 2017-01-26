#include "panel.h"
#include "common.h"
#include "lines/line.h"

#ifndef STARBURSTTEST
#define STARBURSTTEST

namespace mingtest
{

struct starburst_test : public itester
{
    starburst_test (void) {}
    starburst_test (size_t length, size_t nlines) :
        lineLength_(length), nlines_(nlines) {}

    virtual void exec (std::vector<glib::ishaper*>& shapers, POINT& topcorner, size_t width, size_t height)
    {
        assert(width >= lineLength_ * 2 && height >= lineLength_ * 2); // length requirement
        size_t centerx = topcorner.first + width/2;
        size_t centery = topcorner.second + height/2;
        POINT origin(centerx, centery);
        std::vector<POINT> pts = starburst_vertices(centerx, centery, lineLength_, nlines_);
        size_t i = 0;
        for (POINT p : pts)
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

}

#endif /* STARBURSTTEST */