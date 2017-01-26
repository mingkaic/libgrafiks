#include "panel.h"
#include "lines/line.h"

#ifndef PARALLELOTEST
#define PARALLELOTEST

namespace mingtest
{

struct paralleogram_test : public itester
{
    paralleogram_test (void) {}
    paralleogram_test (size_t nparalells) : nparallel_(nparalells) {}

    virtual void exec (std::vector<glib::ishaper*>& shapers, POINT& topcorner, size_t width, size_t height)
    {
        double x = topcorner.first;
        double y = topcorner.second;
        for (size_t i = 0; i < nparallel_; i++)
        {
            glib::line_model line0(POINT(x+20, y+80+i), POINT(x+150, y+150+i));
            glib::line_model line1(POINT(x+160+i, y+270), POINT(x+240+i, y+40));
            shapers[i % shapers.size()]->draw(&line0);
            shapers[i % shapers.size()]->draw(&line1);
        }
    }

private:
    size_t nparallel_ = 50;
};

}

#endif /* PARALLELOTEST */