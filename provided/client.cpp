#include <iostream>
#include "client.h"

#define CANVASW 750
#define CANVASH 750

Client::Client(Drawable *drawable) :
	test(new a2test(CANVASW, CANVASH)),
	drawable(drawable) {}

#define BG_COLOR 0xffffffff
#define PANEL_COLOR 0x00000000

void Client::nextPage() {
    static int pageNumber = 0;
    std::cout << "PageNumber " << pageNumber << std::endl;

	draw_rect(0, 0, CANVASW, CANVASH, BG_COLOR);
	test->run(pageNumber, drawable,
	[this](const panel& p)
	{
		std::pair<glib::point, glib::point> pts = p.get_pts();
		draw_rect(pts.first.getX(), pts.first.getY(), pts.second.getX(), pts.second.getY(), PANEL_COLOR);
	});

    pageNumber++;
}

void Client::draw_rect(int x1, int y1, int x2, int y2, unsigned int color) {
	for(int x = x1; x<x2; x++) {
		for(int y=y1; y<y2; y++) {
			drawable->setPixel(x, y, color);
		}
	}
}
