//
// Created by Mingkai Chen on 2017-02-18.
//

#include "lib/include/lines/ddanalyzer.h"
#include "lib/include/lines/bresenham.h"
#include "lib/include/lines/antialias.h"
#include "lib/include/polygons/convexfiller.h"

#include "test/assignment1/starburst.h"
#include "test/assignment1/parallelogram.h"
#include "test/assignment1/randomtest.h"
#include "test/shared/polygontest.h"

#include "test/tester.h"

#ifndef LIBGRAFIKS_A1TEST_H
#define LIBGRAFIKS_A1TEST_H

class a1test : public tester
{
public:
	// input: canvas width, height, and number of panels_
	a1test(size_t width, size_t height) :
		tester(width, height, 4) {}

	virtual void run (size_t testid, Drawable* drawable, PANEL_DRAW panel_drawer) const
	{
		// line tests
		starburst_test st(125, 90);
		paralleogram_test pt(50);
		random_test rt(30);

		// polygon tests
		polygon_test polyt;

		double opacity = 0.61;

        DRAW draw = [drawable](int x, int y, double, double, unsigned int color, glib::normal&)
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
		glib::convex_filler pol2(
        [this, opacity, drawable](int x, int y, double, double, unsigned int color, glib::normal&)
		{
			unsigned int oldColor = drawable->getPixel(x, y);
			unsigned int newColor = glib::opacity_transform(color, opacity) + glib::opacity_transform(oldColor, (1-opacity));
			drawable->setPixel(x, y, newColor);
		});

		panel_drawer(this->panels_[0]);
		panel_drawer(this->panels_[1]);
		panel_drawer(this->panels_[2]);
		panel_drawer(this->panels_[3]);
		switch(testid % 5) {
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
		}
		drawable->updateScreen();
	}

	void test(itester& t, SHAPERS v1, SHAPERS v2, SHAPERS v3, SHAPERS v4) const
	{
		panels_[0].test(t, v1);
		panels_[1].test(t, v2);
		panels_[2].test(t, v3);
		panels_[3].test(t, v4);
	}
};

#endif //LIBGRAFIKS_A1TEST_H
