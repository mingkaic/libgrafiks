#include "lib/include/lines/antialias.h"

#ifdef __GLIB_AALIAS__

namespace glib
{

struct circle_integration
{
	// maps secant distance from circle edge to percent area covered by secant from edge
	// integral(sqrt(1 - x^2)) = (x*sqrt(1 - x^2) + arcsin(x))/2
	circle_integration (void)
	{
		double min = -1;
		for (size_t i = 0; i < 256; i++)
		{
			double x = min + (double)(i+1) / 128;
			area[i] = ((x*std::sqrt(1-x*x) + std::asin(x)) / 2 + 0.785398163)/1.57079632679;
		}
	}
	double get_area (size_t idx) const { return area[idx]; }

private:
	double area[256]; // covers 256 buckets
};

const double pix_radius = std::sqrt(2)/2;
const circle_integration ctable;

aalias_liner::aalias_liner (DRAW draw) : iliner(draw) {}

void aalias_liner::draw (const line_model* model) const
{
	const transformation& forward = this->octantize(*model);
	transformation backward = forward.transpose();
	double dummyz = 1;

	double dx = model->dx();
	double dy = model->dy();
	double dz = model->dz();
	color_grad dc = model->dc();

	forward.mul(dx, dy, dummyz); // transform to oct1

	double mz = dz / dx;
	color_grad mc = dc / dx;

	double m = dy / dx;

	point origin = model->get_v(0);
	double centerx = origin.getX();
	double centery = origin.getY();
	double centerz = origin.getZ();
	color_grad centercolor = origin.basecolor;
	normal centern = origin.n;
	// we need a representation of the line
	// t is the tangent slope at every point along the line
	// dist from (x, y) to line center is abs(y_int - y_model) * cos(arctan(m))
	double t = std::cos(std::atan(m));

	// since the line width is less than the pixel model,
	// no more than 4 pixels can encompass a line model (between 0 and 45 degrees),
	// one or two on either side (above and below line center) and one very close to the center

	// since we have the slope, approach exactly like dda
	double diff = centerx - std::round(centerx);
	centerx = centerx - diff; // round x
	centery += diff * m; // move y to match rounded x

	double y = centery - std::round(centery); // y is the center of the line relative to the closest pixel
	for (size_t x = 0; x <= dx; ++x) {
		// at everypoint x, we know the y center point of the line exactly
		// consider pixel centers above and below y
		double x0 = (double)x, y0 = (double)std::round(y);
		double x1 = (double)x, y1 = (double)y0+1;
		double x2 = (double)x, y2 = (double)y0-1;
		double x3 = (double)x, y3 = (double)y0;
		// y_int - y_model
		double dc0 = y0 - y;
		double dc1 = dc0 + 1; // top pixel distance from center
		double dc2 = dc0 - 1; // bottom pixel distance from center
		backward.mul(x0, y0, dummyz);
		backward.mul(x1, y1, dummyz);
		backward.mul(x2, y2, dummyz);
		this->drawable_(centerx + x0, centery + y0, centerz, centerz,
			opacity_transform((unsigned)centercolor, intensity(std::abs(dc0) * t)), centern);
		this->drawable_(centerx + x1, centery + y1, centerz, centerz,
			opacity_transform((unsigned)centercolor, intensity(std::abs(dc1) * t)), centern);
		this->drawable_(centerx + x2, centery + y2, centerz, centerz,
			opacity_transform((unsigned)centercolor, intensity(std::abs(dc2) * t)), centern);
		double opacity;
		// look 2 pixels above center if pixel center is below model center
		// above pixels (that are not pixel centers) are closer to mode centers
		if (dc0 < 0)
		{
			y3 += 2;
			opacity = intensity(std::abs(dc0 + 2) * t);
		}
		else
		{
			y3 -= 2;
			opacity = intensity(std::abs(dc0 - 2) * t);
		}
		backward.mul(x3, y3, dummyz);
		this->drawable_(centerx + x3, centery + y3, centerz, centerz,
			opacity_transform(centercolor, opacity), centern);

		y += m;
		// todo: improve extra dimensional interpolation
		centerz += mz; // z is interpolated on a vertical strip (should be rounded wrt to the line perpendicular)
		centercolor += mc; // color is interpolated on a vertical strip (relative to its octant 1 equivalent)
	}
}

double aalias_liner::intensity (double dlcenter) const
{
	double d = std::round(255 - 255*dlcenter/(2*pix_radius));
	if (d <= 0) return 0;
	// equation of a circle is x^2 + y^2 = r^2 -> y = sqrt(r^2 - x^2)
	// take integral at x over area of half circle as percentage of overlap
	// use calibration table built at compile time
	return ctable.get_area((size_t)d);
}

}

#endif
