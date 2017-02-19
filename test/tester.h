//
// Created by Mingkai Chen on 2017-02-18.
//

#include "panel.h"
#include "drawable.h"

#ifndef LIBGRAFIKS_TESTER_H
#define LIBGRAFIKS_TESTER_H

#define SHAPERS std::vector<glib::ishaper*>
#define PANEL_DRAW std::function<void(const panel&)>
#define PADDING 50

class tester
{
public:
	// input: canvas width, height, and number of panels
	tester (size_t width, size_t height, size_t npanel)
	{
		std::pair<size_t,size_t> ndims = greatfactors(npanel);
		size_t ncol = ndims.first;
		size_t nrow = ndims.second;
		size_t pwidth = (width - (ncol + 1) * PADDING) / ncol;
		size_t pheight = (height - (nrow + 1) * PADDING) / nrow;
		for (size_t x = PADDING; x < width-PADDING; x += pwidth + PADDING) {
			for (size_t y = PADDING; y < height-PADDING; y += pheight + PADDING) {
				panels_.push_back(panel(glib::point(x,y), glib::point(x+pwidth,y+pheight)));
			}
		}
	}

	virtual ~tester (void) {}

	virtual void run (size_t testid, Drawable* drawable, PANEL_DRAW panel_drawer) const = 0;

protected:
	std::vector<panel> panels_;

private:
	std::pair<size_t,size_t> greatfactors (size_t n) {
		size_t r = std::sqrt(n);
		size_t i = r;
		while (i > 0 && n % i != 0) {
			i--;
		}
		return {i, n/i};
	}
};

#endif //LIBGRAFIKS_TESTER_H
