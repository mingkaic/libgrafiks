#include "gutils.h"
#include "shaper.h"

#ifndef MTESTPANEL
#define MTESTPANEL

namespace mingtest
{

struct itester
{
    virtual void exec (std::vector<glib::ishaper*>& shapers, POINT& topcorner, size_t width, size_t height) = 0;
};

class panel
{
public:
    panel (POINT corner1, POINT corner2) :
        topleft_(corner1),
        botright_(corner2)
    {
        glib::inorder(topleft_, botright_);
    }

    void test (itester& t, std::vector<glib::ishaper*> liner)
    {
        t.exec(liner, topleft_, 
            botright_.first - topleft_.first,
            botright_.second - topleft_.second);
    }

    std::pair<POINT, POINT> get_pts (void) { return std::pair<POINT, POINT>(topleft_, botright_); }

private:
    POINT topleft_;
    POINT botright_;
};

}

#endif /* MTESTPANEL */