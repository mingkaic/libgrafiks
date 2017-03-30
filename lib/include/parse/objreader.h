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
	obj_reader (std::string path);
	~obj_reader (void);

	virtual void parse (DRAW drawer);

	void get_objects (std::vector<poly_model*>& out);

	color basecolor_ = 0xffffffff;

protected:
	virtual std::unordered_set<char> whiteset (void) const
	{
		return {' ', '\n', '\t', '\r'};
	}
	virtual void tokenize (std::istream& s);

private:
	std::vector<double> factors (std::string lexeme, const std::unordered_set<char>& ignore) const;

#define LEX_TOK std::pair<std::string, size_t>
	std::queue<LEX_TOK> lextok_;
	std::vector<poly_model*> objects_;
};

}

#endif //LIBGRAFIKS_OBJREADER_HPP
