//
// Created by Mingkai Chen on 2017-02-06.
//

#include <list>
#include <string>
#include <fstream>
#include <unordered_set>

#ifndef LIBGRAFIKS_IFREADER_HPP
#define LIBGRAFIKS_IFREADER_HPP

namespace glib
{

class ifreader
{
protected:
	// USEFUL HELPER FUNCTIONS FOR TOKENIZER
	virtual std::unordered_set<char> whiteset (void) const = 0;

	// return true if look ahead matches match
	// lookahead will increment index when match regardless
	// will only increment when unmatched if jmp == true
	bool lookahead (std::istream& s, std::list<char>& q, std::string match, bool jmp = true) const;

	// return string accumulation to the next occurrence of anything in end
	std::string find_first_of (std::istream& s, std::list<char>& q, std::unordered_set<char> end) const;

	// return desired delimiter separated string in the format ...delim...delim...whitespace
	std::string delimited (std::istream& s, std::list<char>& q, char delim, size_t ndelims) const;

	virtual void tokenize (std::istream& s) = 0;

	virtual void parse (void) = 0;
};

} // namespace glib

#endif /* LIBGRAFIKS_IFREADER_HPP */
