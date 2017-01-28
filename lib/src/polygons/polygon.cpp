#include "polygons/polygon.h"

#ifdef __GLIB_POLYGON__

namespace glib
{

poly_model::poly_model (void) {}

poly_model::poly_model (unsigned int color)
{
    this->color_ = color;
}

poly_model::poly_model (std::vector<POINT> vertices)
{
    for (POINT pt : vertices)
    {
        set_v(pt);
    }
}

poly_model::poly_model (std::vector<POINT> vertices, unsigned int color)
{
    this->color_ = color;
    for (POINT pt : vertices)
    {
        set_v(pt);
    }
}

std::vector<POINT> poly_model::ysortindices (void) const
{
    size_t nvs = this->vertices_.size();
    if (nvs == 0) return {};
    std::vector<POINT> ysv = this->vertices_;
    std::sort(ysv.begin(), ysv.end(), [](POINT pt1, POINT pt2)
    {
        return pt1.second > pt2.second;
    });
    return ysv;
}

bool poly_model::cclockwise (void) const
{
    size_t nvs = this->vertices_.size();
    // line polygons models have width 0, never draw; use lines instead
    if (nvs < 3) return false;
    // use shoelace formula
    double sum = 0;
    size_t i = 0;
    for (i = 0; i < nvs - 1; ++i)
    {
        sum += (this->vertices_[i].first - this->vertices_[i+1].first) * 
                (this->vertices_[i].second + this->vertices_[i+1].second);
    }
    sum += (this->vertices_[i].first - this->vertices_[0].first) * 
            (this->vertices_[i].second + this->vertices_[0].second);
    return sum < 0;
}

bool poly_model::isconvex (void) const
{
    // TODO: implement
    return true;
}

ipolygoner::ipolygoner (DRAW drawable) : ishaper(drawable) {}

void ipolygoner::draw (const imodel* model) const
{
    draw(static_cast<const poly_model*>(model));
}

}

#endif