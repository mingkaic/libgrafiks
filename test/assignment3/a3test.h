//
// Created by Mingkai Chen on 2017-03-19.
//

#include "test/tester.h"
#ifndef LIBGRAFIKS_A3TEST_H
#define LIBGRAFIKS_A3TEST_H

class a3test : public tester
{
public:
	static std::string simpfile;

	a3test(size_t width, size_t height) :
	width_(width), height_(height),
			tester(width, height, 1)
	{
		zbuffer = new size_t[width * height];
		srand(time(NULL));
	}

	~a3test (void)
	{
		delete[] zbuffer;
	}

	virtual void run (size_t testid, Drawable* drawable, PANEL_DRAW panel_drawer) const
	{
		size_t pwidth = panels_[0].get_width();
		size_t pheight = panels_[0].get_height();

		DRAW zbufferdepthgreen =
			[this, drawable, pwidth, pheight](int x, int y, int z, unsigned c)
			{
				size_t idx = y * width_ + x;
				if (x > PADDING && y >=PADDING &&
					x < PADDING + pwidth &&
					y < PADDING + pheight &&
					z < zbuffer[idx])
				{
					double zz = 1 + (double) z;
					glib::color cc(0xff00ff00);
					drawable->setPixel(x, y, unsigned(cc * zz));
					zbuffer[idx] = z;
				}
			};

		std::fill(zbuffer, zbuffer + (width_ * height_), 200);
		panel_drawer(this->panels_[0]);
		glib::simp_reader reader(a3test::simpfile, zbufferdepthgreen);
		reader.execute({(double)width_/2, (double)height_/2, 200}, pwidth, pheight);
		drawable->updateScreen();
	}

private:
	size_t* zbuffer;
	size_t width_;
	size_t height_;
};

#endif //LIBGRAFIKS_A3TEST_H
