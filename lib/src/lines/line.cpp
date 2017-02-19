#include "lines/line.h"

#ifdef __GLIB_LINE__

namespace glib
{

static double o2[4][4] = {
	0.0, 1.0, 0.0, 0.0,
	1.0, 0.0, 0.0, 0.0,
	0.0, 0.0, 0.0, 0.0,
	0.0, 0.0, 0.0, 0.0,
};

static double o8[4][4] = {
	1.0, 0.0, 0.0, 0.0,
	0.0, -1.0, 0.0, 0.0,
	0.0, 0.0, 0.0, 0.0,
	0.0, 0.0, 0.0, 0.0,
};

static double o7[4][4] = {
	0.0, -1.0, 0.0, 0.0,
	1.0, 0.0, 0.0, 0.0,
	0.0, 0.0, 0.0, 0.0,
	0.0, 0.0, 0.0, 0.0,
};

static transformation t2(o2);
static transformation t1;
static transformation t8(o8);
static transformation t7(o7);

line_model::line_model (point a, point b)
{
    inorder(a, b);
    this->vertices_ = {a, b};
}

line_model::line_model (point a, point b, unsigned int color) : line_model(a, b)
{
    this->color_ = color;
}

double line_model::dx (void) const
{
    return vertices_[1].x - vertices_[0].x;
}

double line_model::dy (void) const
{
    return vertices_[1].y - vertices_[0].y;
}

iliner::iliner (DRAW drawable) : ishaper(drawable) {}

void iliner::draw (const shape_model* model) const
{
    draw(static_cast<const line_model*>(model));
}

const transformation& iliner::octantize (const line_model& model) const
{
    double m = model.dy() / model.dx(); 
    if (m > 1) // o2
    {
        return t2;
    }
    else if (m < 0)
    {
        if (m > -1) // o8
        {
            return t8;
        }
        else // o7
        {
            return t7;
        }
    }
    return t1;
}

}

#endif