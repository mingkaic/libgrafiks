#include "test/panel.h"
#include "test/assignment1/common.h"
#include "lib/include/polygons/polygon.h"

#include <random>

#ifndef POLYTEST
#define POLYTEST

struct polygon_test : public itester
{
	uint32_t randomColor (void)
	{
		std::random_device rd;
		std::mt19937 gen(rd());
		std::uniform_int_distribution<uint32_t> dis(0xff0f0f0f, 0xffffffff);
		return dis(gen);
	}

	size_t scene = 0;
	virtual void exec (std::vector<glib::ishaper*>& shapers, const glib::point& topcorner, size_t width, size_t height)
	{
		std::random_device rd;
		std::mt19937 gen(rd());
		size_t edgex = (size_t)topcorner.getX();
		size_t edgey = (size_t)topcorner.getY();
		glib::ishaper* shaper = shapers[0];

		switch (scene)
		{
			case 0: // starburst
			{
				size_t centerx = (size_t)(edgex + width / 2);
				size_t centery = (size_t)(edgey + height / 2);
				glib::point origin((double) centerx, (double) centery);
				std::vector<glib::point> pts = starburst_vertices(centerx, centery, 125, 90);
				size_t i = 0;
				for (i = 0; i < pts.size() - 1; i++)
				{
					pts[i].basecolor = randomColor();
					pts[i+1].basecolor = randomColor();
					origin.basecolor = randomColor();
					glib::poly_model poly({origin, pts[i], pts[i+1]});
					shaper->draw(&poly);
				}
				glib::point p1 = pts.back();
				glib::point p2 = pts.front();
				p1.basecolor = randomColor();
				p2.basecolor = randomColor();
				origin.basecolor = randomColor();
				glib::poly_model poly({origin, p1, p2});
				shaper->draw(&poly);
			}
			break;
			case 1: // square test
			{
				const double margin = 5; // 5 px
				double sqrwidth = (width-10*margin)/9;
				double sqrheight = (height-10*margin)/9;
				for (size_t i = 0; i < 9; i++)
				{
					double x = margin * (i+1) + sqrwidth * i;
					for (size_t j = 0; j < 9; j++)
					{
						double y = margin * (j+1) + sqrheight * j;
						glib::point topleft(edgex + x, edgey + y);
						glib::point topright(edgex + x + sqrwidth, edgey + y);
						glib::point botleft(edgex + x, edgey + y + sqrheight);
						glib::point botright(edgex + x + sqrwidth, edgey + y + sqrheight);
						topleft.basecolor = randomColor();
						topright.basecolor = randomColor();
						botright.basecolor = randomColor();
						glib::poly_model toptri({topleft, botright, topright});
						topleft.basecolor = randomColor();
						botleft.basecolor = randomColor();
						botright.basecolor = randomColor();
						glib::poly_model bottri({topleft, botleft, botright});
						shaper->draw(&toptri);
						shaper->draw(&bottri);
					}
				}
			}
			break;
			case 2: // crazy square test
			{
				std::uniform_real_distribution<> dis(-12, 12);
				const double margin = 5; // 5 px
				double sqrwidth = (width-10*margin)/9;
				double sqrheight = (height-10*margin)/9;
				for (size_t i = 0; i < 9; ++i)
				{
					double x = margin * (i+1) + sqrwidth * i;
					for (size_t j = 0; j < 9; ++j)
					{
						double y = margin * (j+1) + sqrheight * j;
						glib::point topleft(dis(gen) + edgex + x, dis(gen) + edgey + y);
						glib::point topright(dis(gen) + edgex + x + sqrwidth, dis(gen) + edgey + y);
						glib::point botleft(dis(gen) + edgex + x, dis(gen) + edgey + y + sqrheight);
						glib::point botright(dis(gen) + edgex + x + sqrwidth, dis(gen) + edgey + y + sqrheight);
						topleft.basecolor = randomColor();
						topright.basecolor = randomColor();
						botright.basecolor = randomColor();
						glib::poly_model toptri({topleft, botright, topright});
						topleft.basecolor = randomColor();
						botleft.basecolor = randomColor();
						botright.basecolor = randomColor();
						glib::poly_model bottri({topleft, botleft, botright});
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
					glib::point p1(edgex + dis(gen), edgey + dis(gen));
					glib::point p2(edgex + dis(gen), edgey + dis(gen));
					glib::point p3(edgex + dis(gen), edgey + dis(gen));
					p1.basecolor = randomColor();
					p2.basecolor = randomColor();
					p3.basecolor = randomColor();

					glib::poly_model poly({p1, p2, p3});
					shaper->draw(&poly);
				}
			}
			break;
		}
		scene++;
	}
};

#endif /* POLYTEST */
