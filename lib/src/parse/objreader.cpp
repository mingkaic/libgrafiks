//
// Created by Mingkai Chen on 2017-03-20.
//

#include "lib/include/parse/objreader.h"

#ifdef LIBGRAFIKS_OBJREADER_HPP

namespace glib
{

enum OBJ_TOK
{
	INVALID = 0,
	VERTEX,
	VERTEX_NORMAL,
	FACE
};

obj_reader::obj_reader (std::string path)
{
	std::ifstream fs(path);
	tokenize(fs);
	fs.close();
}

obj_reader::~obj_reader (void)
{
	for (poly_model* model : objects_)
	{
		delete model;
	}
}

void obj_reader::tokenize (std::istream& s)
{
	std::unordered_set<char> whitespace = whiteset();
	std::list<char> lhqueue; // look ahead queue
	while (s.good())
	{
		char c;
		if (lhqueue.empty())
		{
			c = s.get();
		}
		else
		{
			c = lhqueue.front();
			lhqueue.pop_front();
		}
		std::string lexeme = "";
		OBJ_TOK token = INVALID;
		switch(c)
		{
			case '#': // comment
				// skip until next new line
				exhaust_until(s, lhqueue, {'\n'});
				break;
			case 'v': // vertex or vertex normal
				if (this->lookahead(s, lhqueue, "n", false))
				{
					token = VERTEX_NORMAL;
					lexeme = exhaust_until(s, lhqueue, {'#', '\n'});
				}
				else if (this->lookahead(s, lhqueue, "t", false))
				{
					// treat vt as a comment
					exhaust_until(s, lhqueue, {'#', '\n'});
				}
				else if (this->lookahead(s, lhqueue, " "))
				{
					token = VERTEX;
					lexeme = exhaust_until(s, lhqueue, {'#', '\n'});
				}
				break;
			case 'f': // face
				if (this->lookahead(s, lhqueue, " "))
				{
					token = FACE;
					lexeme = exhaust_until(s, lhqueue, {'#', '\n'});
				}
				break;
			default: // ignore
				break;
		}
		if (token != INVALID) {
			lextok_.push({lexeme, token});
		}
	}
}

std::vector<double> obj_reader::factors (std::string lexeme, const std::unordered_set<char>& ignore) const
{
	std::vector<std::string> vals = this->split(lexeme, " ");
	std::vector<double> sfactors;
	for (std::string sval : vals)
	{
		std::string propers = this->trim(sval, ignore);
		if (!propers.empty())
		{
			sfactors.push_back(std::atof(propers.data())); // doesn't check for non-numerics
		}
	}
	return sfactors;
}

// instructions are either models or transformations
void obj_reader::parse (DRAW drawer)
{
	std::unordered_set<char> whitespace = whiteset();
	std::vector<point> pts;
	std::vector<normal> norms;

	while (false == lextok_.empty())
	{
		LEX_TOK lt = lextok_.front();
		std::string lexeme = lt.first;
		OBJ_TOK token = (OBJ_TOK) lt.second;
		lextok_.pop();
		switch (token)
		{
			case VERTEX:
			{
				std::vector<double> sfactors = factors(lexeme, whitespace);
				size_t nv = sfactors.size();
				if (nv != 3 && nv != 4 && nv != 6 && nv != 7)
				{
					throw std::exception(); // todo: better exception: invalid syntax
				}
				double x = sfactors[0];
				double y = sfactors[1];
				double z = sfactors[2];
				point p(x, y, z);
				if (nv == 4 || nv == 7)
				{
					p.h = sfactors[3];
				}
				if (nv == 6 || nv == 7)
				{
					double r = sfactors[nv-3];
					double g = sfactors[nv-2];
					double b = sfactors[nv-1];
					p.basecolor = color((uint8_t) 255 * r, (uint8_t) 255 * g, (uint8_t) 255 * b);
				}
				else
				{
					p.basecolor = basecolor_;
				}
				pts.push_back(p);
			}
				break;
			case VERTEX_NORMAL:
			{
				std::vector<double> sfactors = factors(lexeme, whitespace);
				size_t nv = sfactors.size();
				if (nv != 3)
				{
					throw std::exception(); // todo: better exception: invalid syntax
				}
				double x = sfactors[0];
				double y = sfactors[1];
				double z = sfactors[2];
				norms.push_back(normal(x, y, z));
			}
				break;
			case FACE:
			{
				std::vector<std::string> vals = this->split(lexeme, " ");
				size_t nv = vals.size();
				if (nv < 3)
				{
					throw std::exception(); // todo: better exception: invalid syntax
				}
				std::vector<point> vts;

				// each val can be <num> or <num>//<num>
				for (std::string val : vals)
				{
					std::string propers = this->trim(val, whitespace);
					if (!propers.empty())
					{
						std::vector<std::string> nums = this->split(propers, "/");
						int vidx = std::atoi(this->trim(nums[0], whitespace).data());
						point p;
						if (vidx < 0)
						{
							p = pts[pts.size() + vidx];
						}
						else
						{
							p = pts[vidx - 1];
						}
						if (nums.size() >= 2)
						{
							int nidx = std::atoi(this->trim(nums.back(), whitespace).data());
							if (nidx < 0)
							{
								p.n = norms[norms.size() + nidx];
							}
							else
							{
								p.n = norms[nidx - 1];
							}
						}
						vts.push_back(p);
					}
				}
				// triangular points to avoid concave faces
				point& first = vts[0];
				auto it = ++vts.begin();
				auto et = vts.end();
				while (it+1 != et)
				{
					objects_.push_back(
                        new poly_model(std::vector<point>{first, *it, *(++it)}));
				}
			}
				break;
			default: // ignore INVALID
				break;
		}
	}
}

void obj_reader::get_objects (std::vector<poly_model*>& out)
{
	out = std::move(objects_); // transfer ownership
}

}

#endif
