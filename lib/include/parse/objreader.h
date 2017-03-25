//
// Created by Mingkai Chen on 2017-03-20.
//

#include <queue>

#include "lib/include/polygons/polygon.h"

#include "lib/include/parse/ifreader.h"

#ifndef LIBGRAFIKS_OBJREADER_HPP
#define LIBGRAFIKS_OBJREADER_HPP

namespace glib
{

class obj_reader : public ifreader
{
public:
	obj_reader (std::string path, color basecolor);
	~obj_reader (void);
	void get_objects (std::vector<poly_model*>& out)
	{
		out = std::move(objects_); // transfer ownership
	}

protected:
	virtual std::unordered_set<char> whiteset (void) const
	{
		return {' ', '\n', '\t', '\r'};
	}
	virtual void tokenize (std::istream& s);
	virtual void parse (DRAW drawer);

private:
	std::vector<double> factors (std::string lexeme, const std::unordered_set<char>& ignore) const;

#define LEX_TOK std::pair<std::string, size_t>
	std::queue<LEX_TOK> lextok_;
	std::vector<poly_model*> objects_;
	color basecolor_;
};

}

#endif //LIBGRAFIKS_OBJREADER_HPP
