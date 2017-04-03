//
// Created by Mingkai Chen on 2017-02-18.
//

#include <ctime>

#include "lib/include/parse/simpreader.h"

#include "test/shared/polygontest.h"
#include "test/assignment2/triangletest.h"

#include "test/tester.h"
#include <iostream>
#ifndef LIBGRAFIKS_A2TEST_H
#define LIBGRAFIKS_A2TEST_H

class a2test : public tester
{
public:
	static std::string testdir;

	// input: canvas width, height, and number of panels_
	a2test(size_t width, size_t height) :
		width_(width), height_(height),
		tester(width, height, 1)
	{
		zbuffer = new size_t[width * height];
		srand(time(NULL));
	}

	~a2test (void)
	{
		delete[] zbuffer;
	}

	virtual void run (size_t testid, Drawable* drawable, PANEL_DRAW panel_drawer) const
	{
		double opacity = 0.61;
		DRAW translucent =
		[this, opacity, drawable](int x, int y, int z, unsigned c, glib::normal& n)
		{
			unsigned int oldColor = drawable->getPixel(x, y);
			unsigned int newColor = glib::opacity_transform(c, opacity) + glib::opacity_transform(oldColor, (1-opacity));
			drawable->setPixel(x, y, newColor);
		};

		DRAW zbuffered =
		[this, drawable](int x, int y, int z, unsigned c, glib::normal& n)
		{
			size_t idx = y * width_ + x;
			if (z < zbuffer[idx])
			{
				drawable->setPixel(x, y, c);
				zbuffer[idx] = z;
			}
		};

		size_t pwidth = panels_[0].get_width();
		size_t pheight = panels_[0].get_height();

		DRAW zbufferdepthred =
		[this, drawable, pwidth, pheight](int x, int y, int z, unsigned c, glib::normal& n)
		{
			size_t idx = y * width_ + x;
			if (x > PADDING && y >=PADDING &&
				x < PADDING + pwidth &&
				y < PADDING + pheight &&
				z < zbuffer[idx] &&
				z >= 0)
			{
				double zz = (200.0-z)/200;
				glib::color cc(0xffff0000);
				drawable->setPixel(x, y, unsigned(cc * zz));
				zbuffer[idx] = z;
			}
		};

		DRAW zbufferdepthgreen =
		[this, drawable, pwidth, pheight](int x, int y, int z, unsigned c, glib::normal& n)
		{
			size_t idx = y * width_ + x;
			if (x > PADDING && y >=PADDING &&
				x < PADDING + pwidth &&
				y < PADDING + pheight &&
				z < zbuffer[idx] &&
				z >= 0)
			{
				double zz = (200.0-z)/200;
				glib::color cc(0xff00ff00);
				drawable->setPixel(x, y, unsigned(cc * zz));
				zbuffer[idx] = z;
			}
		};

		// polygon tests
		polygon_test polyt;
		regulartri_test regtri;

		// polygons
		glib::convex_wirer wired(translucent);
		glib::convex_filler filled(translucent);
		glib::convex_filler zpoly(zbuffered);

		panel_drawer(this->panels_[0]);
		switch(testid % 8) {
			case 0: // wired test 4, panel 3 from assignment 1
				polyt.scene = 2;
				panels_[0].test(polyt, {&wired});
				break;
			case 1: // filled test 4, panel 3 from assignment 1
				polyt.scene = 2;
				panels_[0].test(polyt, {&filled});
				break;
			case 2: // randomly rotated equilateral triangles
				std::fill(zbuffer, zbuffer + (width_ * height_), 200);
				panels_[0].test(regtri, {&zpoly});
				break;
			case 3: //
			{
				std::fill(zbuffer, zbuffer + (width_ * height_), 200);
				glib::simp_reader reader4(a2test::testdir + "/mypage4.simp");
				reader4.parse(zbufferdepthgreen);
				reader4.execute({(double)width_/2, (double)height_/2, 200}, pwidth, pheight);
			}
				break;
			case 4: //
			{
				std::fill(zbuffer, zbuffer + (width_ * height_), 200);
				glib::simp_reader reader5(a2test::testdir + "/page5.simp");
				reader5.parse(zbufferdepthred);
				reader5.execute({(double)width_/2, (double)height_/2, 200}, pwidth, pheight);
			}
				break;
			// unknown tests
			case 5:
			{
				std::fill(zbuffer, zbuffer + (width_ * height_), 200);
				glib::simp_reader reader6(a2test::testdir + "/test1.simp");
				reader6.parse(zbufferdepthgreen);
				reader6.execute({(double)width_/2, (double)height_/2, 200}, pwidth, pheight);
			}
				break;
			case 6:
			{
				std::fill(zbuffer, zbuffer + (width_ * height_), 200);
				glib::simp_reader reader7(a2test::testdir + "/test2.simp");
				reader7.parse(zbufferdepthred);
				reader7.execute({(double)width_/2, (double)height_/2, 200}, pwidth, pheight);
			}
				break;
			case 7:
			{
				std::fill(zbuffer, zbuffer + (width_ * height_), 200);
				glib::simp_reader reader8(a2test::testdir + "/test3.simp");
				reader8.parse(zbufferdepthgreen);
				reader8.execute({(double)width_/2, (double)height_/2, 200}, pwidth, pheight);
			}
				break;
		}
		drawable->updateScreen();
	}

private:
	size_t* zbuffer;
	size_t width_;
	size_t height_;
};

#endif //LIBGRAFIKS_A2TEST_H
