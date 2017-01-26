#include <iostream>
#include "client.h"
#include "panel.h"
#include "starburst.h"
#include "parallelogram.h"
#include "randomtest.h"
#include "polygontest.h"

#include "lines/ddanalyzer.h"
#include "lines/bresenham.h"
#include "lines/antialias.h"
#include "polygons/convexfiller.h"

Client::Client(Drawable *drawable)
{
    this->drawable = drawable;
    panels.push_back(panel(POINT(50,50), POINT(350,350)));
    panels.push_back(panel(POINT(400,50), POINT(700,350)));
    panels.push_back(panel(POINT(50,400), POINT(350,700)));
    panels.push_back(panel(POINT(400,400), POINT(700,700)));
}

#define BG_COLOR 0xffffffff
#define PANEL_COLOR 0x00000000

void Client::nextPage() {
    static int pageNumber = 0;
    pageNumber++;
    std::cout << "PageNumber " << pageNumber << std::endl;
    // line tests
    starburst_test st(125, 90);
    paralleogram_test pt(50);
    random_test rt(30);

    // polygon tests
    polygon_test polyt;

    double opacity = 0.61;

    DRAW draw = [this](int x, int y, unsigned int color)
    {
        unsigned int oldColor = drawable->getPixel(x, y);
        // take shapes of highest intensity on top
        color = glib::higher_intensity(oldColor, color);
        drawable->setPixel(x, y, color);
    };
    glib::dda_liner l1(draw);
    glib::bresen_liner l2(draw);
    glib::aalias_liner l3(draw);

    glib::convex_filler pol1(draw);
    glib::convex_filler pol2([this, opacity](int x, int y, unsigned int color)
    {
        unsigned int oldColor = drawable->getPixel(x, y);
        unsigned int newColor = glib::opacity_transform(color, opacity) + glib::opacity_transform(oldColor, (1-opacity));
        drawable->setPixel(x, y, newColor);
    });

    draw_rect(0, 0, 750, 750, BG_COLOR);
    switch(pageNumber % 5) {
    case 0: // starburst test
        test(st, {&l1}, {&l2}, {&l1, &l2}, {&l3});
        break;
    case 1: // parallelogram test
        test(pt, {&l1}, {&l2}, {&l1, &l2}, {&l3});
        break;
    case 2: // random test
        test(rt, {&l1}, {&l2}, {&l1, &l2}, {&l3});
        break;
    case 3: // polygon tests
        test(polyt, {&pol1}, {&pol1}, {&pol1}, {&pol1});
        break;
    case 4: // opacity polygon tests
        polyt.scene = 0; // reset scene
        test(polyt, {&pol2}, {&pol2}, {&pol2}, {&pol2});
        break;
    default:
        draw_rect(0, 0, 750, 750, 0xffffffff);
        draw_rect(400, 400, 700, 700, 0xff00ff40);
        drawable->updateScreen();
    }
}

void Client::test(itester& t, SHAPERS v1, SHAPERS v2, SHAPERS v3, SHAPERS v4)
{
    draw_panel(panels[0], PANEL_COLOR);
    draw_panel(panels[1], PANEL_COLOR);
    draw_panel(panels[2], PANEL_COLOR);
    draw_panel(panels[3], PANEL_COLOR);
    panels[0].test(t, v1);
    panels[1].test(t, v2);
    panels[2].test(t, v3);
    panels[3].test(t, v4);
    drawable->updateScreen();
}

void Client::draw_rect(int x1, int y1, int x2, int y2, unsigned int color) {
    for(int x = x1; x<x2; x++) {
        for(int y=y1; y<y2; y++) {
            drawable->setPixel(x, y, color);
        }
    }
}

void Client::draw_panel(panel& p, unsigned int color) {
    std::pair<POINT, POINT> pts = p.get_pts();
    draw_rect(pts.first.first, pts.first.second, pts.second.first, pts.second.second, color);
}