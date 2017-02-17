//
// Created by Mingkai Chen on 2017-02-16.
//

#include "parse/simpreader.h"
#include <iostream>

#ifdef LIBGRAFIKS_SIMPREADER_HPP

namespace glib
{

enum SIMP_TOK {
	INVALID,
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
void simp_reader::tokenize (std::istream& s) {
	std::list<char> lhqueue; // look ahead queue
	while (s.good()) {
		char c = s.get();
		std::string lexeme = "";
		SIMP_TOK token = INVALID;
		switch(c) {
			case '#': // comment
				// skip until next new line
				while (c != '\n') {
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
				if (this->lookahead(s, lhqueue, "cale")) {
					token = SCALE;
					// look at format <x>,<y>,<z>
					lexeme = this->delimited(s, lhqueue, ',', 3);
				}
				break;
			case 'r': // rotate
				if (this->lookahead(s, lhqueue, "otate")) {
					token = ROTATE;
					// look at format <axis>,<angle>
					lexeme = this->delimited(s, lhqueue, ',',2);
				}
				break;
			case 't': // translate
				if (this->lookahead(s, lhqueue, "ranslate")) {
					token = TRANSLATE;
					// look at format <x>,<y>,<z>
					lexeme = this->delimited(s, lhqueue, ',',3);
				}
				break;
			case 'l': // line
				if (this->lookahead(s, lhqueue, "ine")) {
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

void simp_reader::parse (void) {
	while (false == lextok_.empty()) {
		LEX_TOK lt = lextok_.front();
		lextok_.pop();
		std::cout << lt.first << " " << lt.second << "\n";
	}
}

}

#endif
