#include <vector>
#include "drawable.h"
#include "pageturner.h"
#include "test/panel.h"
#include "test/assignment1/a1test.h"
#include "test/assignment2/a2test.h"
#include "test/assignment3/a3test.h"

#ifndef CLIENT_H
#define CLIENT_H

class Client : public PageTurner
{
public:
    Client(Drawable *drawable);
    ~Client()
    {
    	delete test;
    }
    void nextPage();

private:
    Drawable* drawable;
	tester* test;

	void draw_rect(int x1, int y1, int x2, int y2, unsigned int color);
};

#endif // CLIENT_H
