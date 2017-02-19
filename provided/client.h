#ifndef CLIENT_H
#define CLIENT_H
#include <vector>
#include "drawable.h"
#include "pageturner.h"
#include "panel.h"
#include "parse/simpreader.h"
#include "assignment1/a1test.h"

class Client : public PageTurner
{
public:
    Client(Drawable *drawable);
    ~Client() {
    	delete test;
    }
    void nextPage();

private:
    Drawable* drawable;
	tester* test;

	void draw_rect(int x1, int y1, int x2, int y2, unsigned int color);
};

#endif // CLIENT_H
