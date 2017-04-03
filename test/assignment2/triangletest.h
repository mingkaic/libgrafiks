//
// Created by Mingkai Chen on 2017-02-19.
//

#ifndef LIBGRAFIKS_TRIANGLETEST_H
#define LIBGRAFIKS_TRIANGLETEST_H

struct regulartri_test : public itester
{
	regulartri_test (void) {
		colors_ = {1, 0.85, 0.7, 0.55, 0.4, 0.25};
	}
	regulartri_test (size_t length, std::vector<double> colors) :
		lineLength_(length), colors_(colors) {}

	virtual void exec (std::vector<glib::ishaper*>& shapers, const glib::point& topcorner, size_t width, size_t height)
	{
		assert(width >= lineLength_ * 2 && height >= lineLength_ * 2); // length requirement
		size_t x = topcorner.getX() + width/2;
		size_t y = topcorner.getY() + height/2;
		glib::ishaper* shaper = shapers[0];

		for (double color : colors_)
		{
			double z = rand() % 199;
			uint8_t c = 255*color;
			unsigned base = (0xff << 24) + (c << 16) + (c << 8) + c;

			double rotation1 = M_PI * (rand() % 120) / 180; // convert to radians
			double rotation2 = rotation1 + 2 * M_PI / 3;
			double rotation3 = rotation1 + 4 * M_PI / 3;

			double x1 = ((double) lineLength_) * std::cos(rotation1);
			double y1 = ((double) lineLength_) * std::sin(rotation1);
			double x2 = ((double) lineLength_) * std::cos(rotation2);
			double y2 = ((double) lineLength_) * std::sin(rotation2);
			double x3 = ((double) lineLength_) * std::cos(rotation3);
			double y3 = ((double) lineLength_) * std::sin(rotation3);

			glib::point p1(x+x1, y+y1, z);
			glib::point p2(x+x2, y+y2, z);
			glib::point p3(x+x3, y+y3, z);
			p1.basecolor = base;
			p2.basecolor = base;
			p3.basecolor = base;

			glib::poly_model poly({p3, p2, p1});
			shaper->draw(&poly);
		}
	}

private:
	size_t lineLength_ = 275;
	std::vector<double> colors_;
};

#endif //LIBGRAFIKS_TRIANGLETEST_H
