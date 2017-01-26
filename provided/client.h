#ifndef CLIENT_H
#define CLIENT_H
#include <vector>
#include "drawable.h"
#include "pageturner.h"
#include "panel.h"

using namespace mingtest;
#define SHAPERS std::vector<glib::ishaper*>

class Client : public PageTurner
{
public:
    Client(Drawable *drawable);
    void nextPage();

private:
    std::vector<panel> panels;
    Drawable *drawable;

    void draw_rect(int x1, int y1, int x2, int y2, unsigned int color);
    void draw_panel(panel& p, unsigned int color);
    void test(itester& t, SHAPERS v1, SHAPERS v2, SHAPERS s3, SHAPERS v4);
};

#endif // CLIENT_H
