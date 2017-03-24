//
// Created by Mingkai Chen on 2017-02-06.
//

#include <stack>
#include <queue>
#include <unordered_map>

#include "lib/include/parse/ifreader.h"
#include "lib/include/parse/objreader.h"
#include "lib/include/lines/line.h"
#include "lib/include/polygons/polygon.h"
#include "lib/include/models/transformation.h"

#include "lib/include/lines/ddanalyzer.h"
#include "lib/include/polygons/convexwirer.h"
#include "lib/include/polygons/convexfiller.h"

#ifndef LIBGRAFIKS_SIMPREADER_HPP
#define LIBGRAFIKS_SIMPREADER_HPP

namespace glib
{

#define SCREEN_DIM 100

class simp_reader : public ifreader
{
public:
	simp_reader (std::string path, DRAW drawer,
		color_grad ambient = 0,
		color surface = 0xffffffff,
		std::shared_ptr<ishaper> goner = nullptr);
	~simp_reader (void);

	void execute (point centeryon, size_t width, size_t height);

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
		std::unordered_set<char> ignore,
		color def_color) const;

	color_grad to_rgb(std::string cs, std::string delim,
		std::unordered_set<char> ignore) const;

#define RUN_INSTR std::function<void(INSTRUCTION*)>
#define LEX_TOK std::pair<std::string, size_t>

	struct INSTRUCTION
	{
		INSTRUCTION (size_t stackidx) : stack_(stackidx) {}
		virtual ~INSTRUCTION (void) {}

		simp_reader* owner_ = nullptr;
		size_t stack_;
	};

	struct RENDER : public INSTRUCTION
	{
		RENDER (shape_model* model, std::shared_ptr<ishaper>& shape, size_t stackidx = 0) :
			INSTRUCTION(stackidx)
		{
			render_.shaper_ = shape;
			render_.model_ = model;
		}
		shape_render render_;
	};

	struct TRANSFORM : public INSTRUCTION
	{
		TRANSFORM (transformation* t, size_t stackidx = 0) :
			INSTRUCTION(stackidx), trans_(t) {}

		transformation* trans_;
	};

	struct CAMERA_INST : public INSTRUCTION
	{
		CAMERA_INST (
			double left, double right,
			double down, double top,
			double front, double back,
			size_t stackidx = 0) :
			INSTRUCTION(stackidx),
			left_(left), right_(right),
			down_(down), top_(top),
			front_(front), back_(back) {}

		double left_;
		double right_;
		double down_;
		double top_;
		double front_;
		double back_;
	};

	void get_instructions (RUN_INSTR run)
	{
		for (INSTRUCTION* i : instructions_) {
			i->owner_ = this;
			run(i);
		}
	}

	std::string directory_ = "";
	std::queue<LEX_TOK> lextok_;
	std::list<INSTRUCTION*> instructions_;
	std::vector<simp_reader*> subreaders_;

	color_grad ambient_;
	color surface_;
	std::shared_ptr<ishaper> goner_;
};

}

#endif //LIBGRAFIKS_SIMPREADER_HPP
