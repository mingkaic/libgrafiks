//
// Created by Mingkai Chen on 2017-02-17.
//

#include "lib/include/parse/ifreader.h"

#ifdef LIBGRAFIKS_IFREADER_HPP

namespace glib
{

bool ifreader::lookahead (std::istream& s, std::list<char>& q, std::string match, bool jmp) const
{
	size_t nmatch = match.length();
	while (s.good() && q.size() < nmatch)
	{
		q.push_back(s.get());
	}

	std::unordered_set<char> whitespace = whiteset();
	std::string lexeme = "";
	auto it = q.begin();
	auto spaceit = q.end();
	for (size_t i = 0; i < nmatch; i++)
	{
		char c = *it;
		if (q.end() == spaceit && whitespace.end() != whitespace.find(c))
		{
			spaceit = it;
		}
		lexeme.push_back(c);
		it++;
	}
	if (0 == lexeme.compare(match))
	{
		q.erase(q.begin(), spaceit);
		return true;
	}
	if (jmp) {
		q.erase(q.begin(), spaceit);
	}
	return false;
}

std::string ifreader::find_first_of (std::istream& s, std::list<char>& q, std::unordered_set<char> end) const
{
	std::string accum = "";
	char c;
	do
	{
		if (q.empty())
		{
			c = s.get();
		}
		else
		{
			c = q.front();
			q.pop_front();
		}
		accum.push_back(c);
	}
	while (s.good() && end.end() == end.find(c));
	return accum;
}

std::string ifreader::delimited (std::istream& s, std::list<char>& q, std::unordered_set<char> delims, size_t ndelims) const
{
	std::unordered_set<char> whitespace = whiteset();
	std::string accum = "";
	for (size_t i = 0; i < ndelims; i++)
	{
		std::string found = find_first_of(s, q, delims);
		std::string cut = trim(found, delims);
		if (cut.empty()) {
			found = cut;
		}
		accum += found;
		if (found.empty()) {
			ndelims++;
		}
	}
	std::string cut;
	do
	{
		std::string next = find_first_of(s, q, whitespace);
		accum += next;
		cut = trim(next, whitespace);
	}
	while (cut.empty());
	return accum;
}

std::string ifreader::exhaust_until (std::istream& s, std::list<char>& q, std::unordered_set<char> delims) const
{
	std::string accum = "";
	bool found = false;
	while (false == q.empty() && !found)
	{
		if (delims.find(q.front()) != delims.end())
		{
			found = true;
		}
		else
		{
			accum += q.front();
			q.pop_front();
		}
	}
	if (!found)
	{
		while (s.good() && !found)
		{
			char c = s.get();
			if (delims.find(c) != delims.end())
			{
				found = true;
				q.push_back(c);
			}
			else
			{
				accum += c;
			}
		}
	}
	return accum;
}

std::vector<std::string> ifreader::split (std::string s, std::string delim) const
{
	size_t ns = s.size();
	std::vector<std::string> accum;
	size_t next;
	for (next = 0; next < ns; next++)
	{
		size_t prev = next;
		next = s.find_first_of(delim, next);
		if (std::string::npos == next)
		{
			next = ns;
		}
		std::string subs = s.substr(prev, next - prev);
		if (!subs.empty())
		{
			accum.push_back(subs);
		}
	}
	return accum;
}

std::string ifreader::trim (std::string s, std::unordered_set<char> trash) const
{
	std::string accum = "";
	for (char c : s)
	{
		if (trash.end() == trash.find(c))
		{
			accum.push_back(c);
		}
	}
	return accum;
}

}

#endif
