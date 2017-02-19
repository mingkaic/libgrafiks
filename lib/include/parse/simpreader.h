//
// Created by Mingkai Chen on 2017-02-06.
//

#include <queue>
#include <unordered_map>

#include "parse/ifreader.h"
#include "lines/line.h"
#include "polygons/polygon.h"
#include "models/transformation.h"

#include "lines/ddanalyzer.h"
#include "polygons/convexwirer.h"
#include "polygons/convexfiller.h"

#ifndef LIBGRAFIKS_SIMPREADER_HPP
#define LIBGRAFIKS_SIMPREADER_HPP

namespace glib
{

#define RUN_INSTR std::function<void(INSTRUCTION&)>

// todo: upgrade to variant<shape_model, transformation>
struct INSTRUCTION
{
	INSTRUCTION (transformation* t) :
		trans_(t), ismodel_(false) {}
	INSTRUCTION (shape_model* model, std::shared_ptr<ishaper>& shape) :
		ismodel_(true)
	{
		render_.shaper_ = shape;
		render_.model_ = model;
	}

	bool ismodel_;
	shape_render render_;
	transformation* trans_ = nullptr;
};

class simp_reader : public ifreader
{
public:
	simp_reader (std::string path, DRAW drawer)
	{
		size_t nameidx = path.find_last_of('/');
		if (path.npos != nameidx) {
			directory_ = path.substr(0, nameidx);
		}
		std::ifstream fs(path);
		tokenize(fs);
		parse(drawer);
		fs.close();
	}

	void get_instructions (RUN_INSTR run)
	{
		for (INSTRUCTION i : instructions_) {
			run(i);
		}
	}

protected:
	virtual std::unordered_set<char> whiteset (void) const
	{
		return {' ', '\n', '\t', '\r'};
	}

	// implements a pseudo tokenizer that implies partial information on grammar (otherwise we'll need regex for numerics)
	virtual void tokenize (std::istream& s);

	// parses lextok for grammatic meaning based on simp specifications
	virtual void parse (DRAW drawer);

private:
	point to_point(std::string pts, std::string delims,
		std::unordered_set<char> ignore) const;

#define LEX_TOK std::pair<std::string, size_t>

	std::string directory_ = "";
	std::queue<LEX_TOK> lextok_;
	std::list<INSTRUCTION> instructions_;
};

}

#endif //LIBGRAFIKS_SIMPREADER_HPP
