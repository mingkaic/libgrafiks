//
// Created by Mingkai Chen on 2017-02-17.
//

#include "parse/ifreader.h"

#ifdef LIBGRAFIKS_IFREADER_HPP

namespace glib
{

bool ifreader::lookahead (std::istream& s, std::list<char>& q, std::string match, bool jmp) const {
	std::unordered_set<char> whitespace = whiteset();
	bool spaced = false;
	std::string lexeme = "";
	size_t nmatch = match.length();
	auto it = q.begin();
	for (size_t i = 0; i < nmatch; i++) {
		char c;
		if (it != q.end()) {
			c = *it;
		}
		else {
			c = s.get();
			q.push_back(c);
		}
		if (jmp && !spaced && whitespace.end() == whitespace.find(c)) {
			q.pop_front();
		}
		else {
			it++;
		}
		lexeme.push_back(c);
	}
	if (0 == lexeme.compare(match)) {
		return true;
	}
	return false;
}

std::string ifreader::find_first_of (std::istream& s, std::list<char>& q, std::unordered_set<char> end) const {
	std::string accum = "";
	char c;
	do {
		if (q.empty()) {
			c = s.get();
		}
		else {
			c = q.front();
			q.pop_front();
		}
		accum.push_back(c);
	}
	while (end.end() == end.find(c));
	return accum;
}

std::string ifreader::delimited (std::istream& s, std::list<char>& q, char delim, size_t ndelims) const {
	std::unordered_set<char> whitespace = whiteset();
	std::string accum = "";
	for (size_t i = 0; i < ndelims; i++) {
		accum += find_first_of(s, q, {delim});
	}
	accum += find_first_of(s, q, whitespace);
	return accum;
}

}

#endif
