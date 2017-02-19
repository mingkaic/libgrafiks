//
// Created by Mingkai Chen on 2017-02-16.
//

#include "parse/simpreader.h"

#ifdef LIBGRAFIKS_SIMPREADER_HPP

namespace glib
{

enum SIMP_TOK
{
	INVALID = 0,
	PUSH_STACK,
	POP_STACK,
	SCALE,
	ROTATE,
	TRANSLATE,
	LINE,
	POLYGON,
	FILE,
	WIRE,
	FILL
};

// implements a pseudo tokenizer that implies partial information on grammar (otherwise we'll need regex for numerics)
void simp_reader::tokenize (std::istream& s)
{
	std::list<char> lhqueue; // look ahead queue
	while (s.good())
	{
		char c = s.get();
		std::string lexeme = "";
		SIMP_TOK token = INVALID;
		switch(c) {
			case '#': // comment
				// skip until next new line
				while (c != '\n')
				{
					c = s.get();
				}
				break;
			case '{':
				lexeme.push_back(c);
				token = PUSH_STACK;
				break;
			case '}':
				lexeme.push_back(c);
				token = POP_STACK;
				break;
			case 's': // scale
				if (this->lookahead(s, lhqueue, "cale"))
				{
					token = SCALE;
					// look at format <x>,<y>,<z>
					lexeme = this->delimited(s, lhqueue, ',', 3);
				}
				break;
			case 'r': // rotate
				if (this->lookahead(s, lhqueue, "otate"))
				{
					token = ROTATE;
					// look at format <axis>,<angle>
					lexeme = this->delimited(s, lhqueue, ',',2);
				}
				break;
			case 't': // translate
				if (this->lookahead(s, lhqueue, "ranslate"))
				{
					token = TRANSLATE;
					// look at format <x>,<y>,<z>
					lexeme = this->delimited(s, lhqueue, ',',3);
				}
				break;
			case 'l': // line
				if (this->lookahead(s, lhqueue, "ine"))
				{
					token = LINE;
					// look at format (<x>,<y>,<h>)(<x>,<y>,<h>)
					for (size_t i = 0; i < 2; i++) {
						lexeme += this->find_first_of(s, lhqueue, {')'});
					}
				}
				break;
			case 'p': // polygon
				if (this->lookahead(s, lhqueue, "olygon")) {
					token = POLYGON;
					for (size_t i = 0; i < 3; i++) {
						lexeme += this->find_first_of(s, lhqueue, {')'});
					}
				}
				break;
			case 'w': // wire
				if (this->lookahead(s, lhqueue, "ire")) {
					token = WIRE;
					lexeme = "wire";
				}
				break;
			case 'f': // file or fill
				if (this->lookahead(s, lhqueue, "ile", false)) {
					token = FILE;
					// look at format "word"
					for (size_t i = 0; i < 2; i++) {
						lexeme += this->find_first_of(s, lhqueue, {'"'});
					}
				}
				// this look ahead will jump to next space or end of match if match is wrong
				else if (this->lookahead(s, lhqueue, "ill")) {
					token = FILL;
					lexeme = "fill";
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

point simp_reader::to_point(std::string pts, std::string delim,
		std::unordered_set<char> ignore) const
{
	// pts should be in format #,#,#
	std::vector<std::string> vals = this->split(pts, delim);
	if (vals.size() < 3)
	{
		throw std::exception(); // todo: better exception: invalid syntax
	}
	std::vector<double> sfactors;
	for (std::string sval : vals)
	{
		sfactors.push_back(std::atof(this->trim(sval, ignore).data())); // doesn't check for non-numerics
	}
	return point{sfactors[0], sfactors[1], sfactors[2]};
}

// instructions are either models or transformations
void simp_reader::parse (DRAW drawer)
{
	std::unordered_set<char> whitespace = whiteset();
	std::unordered_set<char> filter = whitespace;
	filter.insert('(');
	filter.insert('"');
	size_t pushcount = 0;
	bool polyfill = true;
	std::shared_ptr<ishaper> liner = std::shared_ptr<ishaper>(new dda_liner(drawer));
	std::shared_ptr<ishaper> goner = std::shared_ptr<ishaper>(new convex_filler(drawer));

	while (false == lextok_.empty())
	{
		LEX_TOK lt = lextok_.front();
		lextok_.pop();
		std::string lexeme = lt.first;
		SIMP_TOK token = (SIMP_TOK) lt.second;
		switch (token) {
			case PUSH_STACK:
				pushcount++;
				break;
			case POP_STACK:
				pushcount--;
				break;
			case SCALE:
			{
				point pt = to_point(lexeme, ",", whitespace);
				instructions_.push_back(new scale(pt));
			}
				break;
			case ROTATE: // lexeme should be in format [X,Y,Z],#
			{
				std::vector<std::string> vals = this->split(lexeme, ",");
				if (vals.size() == 2)
				{
					throw std::exception(); // todo: better exception: invalid syntax
				}
				char axis = this->trim(vals[1], whitespace)[0];
				double degrees = std::atof(this->trim(vals[1], whitespace).data()); // doesn't check for non-numerics
				instructions_.push_back(new rotate(axis, degrees));
			}
				break;
			case TRANSLATE:
			{
				point pt = to_point(lexeme, ",", whitespace);
				instructions_.push_back(new translate(pt));
			}
				break;
			case LINE:
			{
				std::vector<std::string> pts = this->split(lexeme, ")");
				if (pts.size() == 3)
				{
					throw std::exception(); // todo: better exception: invalid syntax
				}

				line_model* line = new line_model(
					to_point(pts[0], ",", filter),
					to_point(pts[1], ",", filter));
				instructions_.push_back({line, liner});
			}
				break;
			case POLYGON:
			{
				std::vector<std::string> pts = this->split(lexeme, ")");
				if (pts.size() == 4)
				{
					throw std::exception(); // todo: better exception: invalid syntax
				}
				poly_model* poly = new poly_model({
					to_point(pts[0], ",", filter),
					to_point(pts[1], ",", filter),
					to_point(pts[2], ",", filter)
				});
				instructions_.push_back({poly, goner});
			}
				break;
			case FILE:
			{
				std::string f = this->trim(lexeme, filter);
				f += ".simp";
				// read and parse f
				simp_reader reader(directory_ + f, drawer);
				reader.get_instructions(
				[this](INSTRUCTION& i) {
					instructions_.push_back(i);
				});
			}
				break;
			case WIRE:
				goner = std::shared_ptr<ishaper>(new convex_wirer(drawer));
				break;
			case FILL:
				goner = std::shared_ptr<ishaper>(new convex_filler(drawer));
				break;
			default: // ignore INVALID
				break;
		}
	}
}

}

#endif
