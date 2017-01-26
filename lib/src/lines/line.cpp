#include "lines/line.h"

#ifdef __GLIB_LINE__

namespace glib
{

const coord_transform oct1 = {
    1.0, 0.0, 
    0.0, 1.0
};
const coord_transform oct2 = {
    0.0, 1.0, 
    1.0, 0.0
};
const coord_transform oct8 = {
    1.0, 0.0, 
    0.0, -1.0
};
const coord_transform oct7 = {
    0.0, -1.0, 
    1.0, 0.0
};

line_model::line_model (POINT a, POINT b)
{
    inorder(a, b);
    this->vertices_ = {a, b};
}

line_model::line_model (POINT a, POINT b, unsigned int color) : line_model(a, b)
{
    this->color_ = color;
}

double line_model::dx (void) const
{
    return vertices_[1].first - vertices_[0].first;
}

double line_model::dy (void) const
{
    return vertices_[1].second - vertices_[0].second;
}

iliner::iliner (DRAW drawable) : ishaper(drawable) {}

void iliner::draw (const imodel* model) const
{
    draw(static_cast<const line_model*>(model));
}

const coord_transform& iliner::octantize (const line_model& model) const
{
    double m = model.dy() / model.dx(); 
    if (m > 1) // o2
    {
        return oct2;
    }
    else if (m < 0)
    {
        if (m > -1) // o8
        {
            return oct8;
        }
        else // o7
        {
            return oct7;
        }
    }
    return oct1;
}

}

#endif