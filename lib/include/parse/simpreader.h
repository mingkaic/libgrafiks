//
// Created by Mingkai Chen on 2017-02-06.
//

#include <queue>
#include <unordered_map>
#include "ifreader.h"

#ifndef LIBGRAFIKS_SIMPREADER_HPP
#define LIBGRAFIKS_SIMPREADER_HPP

namespace glib
{

class simp_reader : public ifreader
{
public:
	simp_reader (std::string fname) {
		std::ifstream fs(fname);
		tokenize(fs);
		parse();
		fs.close();
	}

protected:
	virtual std::unordered_set<char> whiteset (void) const {
		return {' ', '\n', '\t', '\r'};
	}

	// implements a pseudo tokenizer that implies partial information on grammar (otherwise we'll need regex for numerics)
	virtual void tokenize (std::istream& s);

	// parses lextok for grammatic meaning based on simp specifications
	virtual void parse (void);

private:
#define LEX_TOK std::pair<std::string, size_t>

	std::queue<LEX_TOK> lextok_;
};

}

#endif //LIBGRAFIKS_SIMPREADER_HPP
