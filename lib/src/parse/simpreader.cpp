//
// Created by Mingkai Chen on 2017-02-16.
//

#include "lib/include/parse/simpreader.h"

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

simp_reader::simp_reader (std::string path, DRAW drawer)
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

simp_reader::~simp_reader (void)
{
	for (INSTRUCTION i : instructions_)
	{
		if (i.owner_ == this || i.owner_ == nullptr)
		{
			if (i.ismodel_)
			{
				delete i.render_.model_;
			}
			else
			{
				delete i.trans_;
			}
		}
	}
	for (simp_reader* subs : subreaders_)
	{
		delete subs;
	}
}

void simp_reader::execute (point centeryon, size_t width, size_t height)
{
	std::stack<transformation> ctfs;
	std::vector<shape_render> shapes;
	ctfs.push(transformation());

	for (INSTRUCTION i : instructions_)
	{
		assert(false == ctfs.empty()); // ctfs may never be empty
		while (i.stack_ < ctfs.size() - 1)
		{
			ctfs.pop();
		}
		while (i.stack_ >= ctfs.size())
		{
			transformation cpy = ctfs.top();
			ctfs.push(cpy);
		}
		transformation& ctf = ctfs.top();
		if (i.ismodel_)
		{
			// transform in order of ctf
			i.render_.model_->transform(ctf);
			shapes.push_back(i.render_);
		}
		else
		{
			ctf = ctf.matmul(*i.trans_);
		}
	}

	// transform wrt to camera
	double halfw = width / 2.0;
	double halfh = height / 2.0;
	scale s({halfw/SCREEN_DIM, halfh/SCREEN_DIM, 1});
	translate t({centeryon.x, centeryon.y});

	// after transformation, we have planes:
	// todo: have a unified representation of planes
	// for now represent by (point, normal)
	point hither = centeryon; hither.z = 0;
	point left = centeryon;
	left.z /= 2; left.x -= halfw;
	point right = centeryon;
	right.z /= 2; right.x += halfw;
	point bot = centeryon;
	bot.z /= 2; bot.y -= halfh;
	point top = centeryon;
	top.z /= 2; top.y += halfh;

	// scale screen to size of window, then translate origin to center
	transformation towindow = t.matmul(s);
	for (shape_render& rends : shapes)
	{
		rends.model_->transform(towindow);
		// clip or cull
		// for now we will rely on draw to perform model on model clipping via zbuffers
		// todo: implement model clipping/culling
		// for now clip wrt to window edge
		// short circuiting allows us to stop clipping once a model is culled
		if (!(
			rends.model_->clip_plane(left, {1, 0, 0}) ||
			rends.model_->clip_plane(right, {-1, 0, 0}) ||
			rends.model_->clip_plane(bot, {0, 1, 0}) ||
			rends.model_->clip_plane(top, {0, -1, 0}) ||
			rends.model_->clip_plane(hither, {0, 0, 1}) ||
			rends.model_->clip_plane(centeryon, {0, 0, -1})
		))
		{
			rends.run();
		}
		// else model is culled
	}
}

// implements a pseudo tokenizer that implies partial information on grammar (otherwise we'll need regex for numerics)
void simp_reader::tokenize (std::istream& s)
{
	std::unordered_set<char> whitespace = whiteset();
	std::list<char> lhqueue; // look ahead queue
	while (s.good())
	{
		char c = s.get();
		std::string lexeme = "";
		SIMP_TOK token = INVALID;
		switch(c) {
			case '#': // comment
				// skip until next new line
				while (s.good() && c != '\n')
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
					lexeme = this->delimited(s, lhqueue, {','}, 2);
				}
				break;
			case 'r': // rotate
				if (this->lookahead(s, lhqueue, "otate"))
				{
					token = ROTATE;
					// look at format <axis> <angle>
					lexeme = this->delimited(s, lhqueue, whitespace, 1);
				}
				break;
			case 't': // translate
				if (this->lookahead(s, lhqueue, "ranslate"))
				{
					token = TRANSLATE;
					// look at format <x>,<y>,<z>
					lexeme = this->delimited(s, lhqueue, {','}, 2);
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
		std::string lexeme = lt.first;
		SIMP_TOK token = (SIMP_TOK) lt.second;
		INSTRUCTION inst;
		lextok_.pop();
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
				inst = {new scale(pt), pushcount};
			}
				break;
			case ROTATE: // lexeme should be in format [X,Y,Z],#
			{
				std::vector<std::string> vals = this->split(lexeme, " ");
				if (vals.size() != 2)
				{
					throw std::exception(); // todo: better exception: invalid syntax
				}
				char axis = this->trim(vals[0], whitespace)[0];
				double degrees = std::atof(this->trim(vals[1], whitespace).data()); // doesn't check for non-numerics
				inst = {new rotate(axis, degrees), pushcount};
			}
				break;
			case TRANSLATE:
			{
				point pt = to_point(lexeme, ",", whitespace);
				inst = {new translate(pt), pushcount};
			}
				break;
			case LINE:
			{
				std::vector<std::string> pts = this->split(lexeme, ")");
				if (pts.size() != 2)
				{
					throw std::exception(); // todo: better exception: invalid syntax
				}

				line_model* line = new line_model(
					to_point(pts[0], ",", filter),
					to_point(pts[1], ",", filter));
				inst = {line, liner, pushcount};
			}
				break;
			case POLYGON:
			{
				std::vector<std::string> pts = this->split(lexeme, ")");
				if (pts.size() != 3)
				{
					throw std::exception(); // todo: better exception: invalid syntax
				}
                std::vector<point> respts = {
                    to_point(pts[0], ",", filter),
                    to_point(pts[1], ",", filter),
                    to_point(pts[2], ",", filter)
                };
                poly_model* poly = new poly_model(respts);
				inst = {poly, goner, pushcount};
			}
				break;
			case FILE:
			{
				std::string f = this->trim(lexeme, filter);
				f += ".simp";
				// read and parse f
				simp_reader* reader = new simp_reader(directory_ + "/" + f, drawer);
				reader->get_instructions(
				[this, pushcount](INSTRUCTION i) {
					i.stack_ += pushcount;
					instructions_.push_back(i);
				});
				subreaders_.push_back(reader);
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
		instructions_.push_back(inst);
	}
}

}

#endif
