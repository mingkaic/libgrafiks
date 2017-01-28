#include "panel.h"
#include "common.h"
#include "polygons/polygon.h"
#include <random>

#ifndef POLYTEST
#define POLYTEST

namespace mingtest
{

struct polygon_test : public itester
{
    unsigned int randomColor (void)
    {
        unsigned int basecolor = std::rand() % 0x00f0f0f0;
        basecolor += 0xff0f0f0f0f; // base color must be above 0x0f per color channel
        return basecolor;
    }

    size_t scene = 0;
    virtual void exec (std::vector<glib::ishaper*>& shapers, POINT& topcorner, size_t width, size_t height)
    {
        std::random_device rd;
        std::mt19937 gen(rd());
        size_t edgex = topcorner.first;
        size_t edgey = topcorner.second;
        glib::ishaper* shaper = shapers[0];

        switch (scene)
        {
            case 0: // starburst
            {
                size_t centerx = edgex + width / 2;
                size_t centery = edgey + height / 2;
                POINT origin(centerx, centery);
                std::vector<POINT> pts = starburst_vertices(centerx, centery, 125, 90);
                size_t i = 0;
                for (i = 0; i < pts.size() - 1; i++)
                {
                    glib::poly_model poly({origin, pts[i], pts[i+1]}, randomColor());
                    shaper->draw(&poly);
                }
                glib::poly_model poly({origin, pts.back(), pts.front()}, randomColor());
                shaper->draw(&poly);
            }
            break;
            case 1: // square test
            {
                const size_t margin = 5; // 5 px
                double sqrwidth = (width-10*margin)/9;
                double sqrheight = (height-10*margin)/9;
                for (size_t i = 0; i < 9; i++)
                {
                    double x = margin * (i+1) + sqrwidth * i;
                    for (size_t j = 0; j < 9; j++)
                    {
                        double y = margin * (j+1) + sqrheight * j;
                        POINT topleft(edgex + x, edgey + y);
                        POINT topright(edgex + x + sqrwidth, edgey + y);
                        POINT botleft(edgex + x, edgey + y + sqrheight);
                        POINT botright(edgex + x + sqrwidth, edgey + y + sqrheight);
                        glib::poly_model toptri({topleft, topright, botright}, randomColor());
                        glib::poly_model bottri({topleft, botleft, botright}, randomColor());
                        shaper->draw(&toptri);
                        shaper->draw(&bottri);
                    }
                }
            }
            break;
            case 2: // crazy square test
            {
                std::uniform_real_distribution<> dis(-12, 12);
                const size_t margin = 5; // 5 px
                double sqrwidth = (width-10*margin)/9;
                double sqrheight = (height-10*margin)/9;
                for (size_t i = 0; i < 9; ++i)
                {
                    double x = margin * (i+1) + sqrwidth * i;
                    for (size_t j = 0; j < 9; ++j)
                    {
                        double y = margin * (j+1) + sqrheight * j;
                        POINT topleft(dis(gen) + edgex + x, dis(gen) + edgey + y);
                        POINT topright(dis(gen) + edgex + x + sqrwidth, dis(gen) + edgey + y);
                        POINT botleft(dis(gen) + edgex + x, dis(gen) + edgey + y + sqrheight);
                        POINT botright(dis(gen) + edgex + x + sqrwidth, dis(gen) + edgey + y + sqrheight);
                        glib::poly_model toptri({topleft, topright, botright}, randomColor());
                        glib::poly_model bottri({topleft, botleft, botright}, randomColor());
                        shaper->draw(&toptri);
                        shaper->draw(&bottri);
                    }
                }
            }
            break;
            case 3: // random test
            {
                std::uniform_real_distribution<> dis(0, 299);
                for (size_t i = 0; i < 20; ++i)
                {
                    glib::poly_model poly({
                        POINT(edgex + dis(gen), edgey + dis(gen)),
                        POINT(edgex + dis(gen), edgey + dis(gen)),
                        POINT(edgex + dis(gen), edgey + dis(gen))
                    }, randomColor());
                    shaper->draw(&poly);
                }
            }
            break;
        }
        scene++;
    }
};

}

#endif /* POLYTEST */