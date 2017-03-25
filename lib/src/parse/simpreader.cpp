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
	FILL,
	CAMERA,
	OBJ,
	AMBIENT,
	DEPTH,
	SURFACE
};

simp_reader::simp_reader (std::string path, DRAW drawer,
	color_grad ambient,
	color surface,
	std::shared_ptr<ishaper> goner) :
	ambient_(ambient),
	surface_(surface),
	goner_(goner)
{
    size_t nameidx = path.find_last_of('/');
    if (path.npos == nameidx) {
        nameidx = path.find_last_of('\\');
    }
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
	for (INSTRUCTION* i : instructions_)
	{
		if (i->owner_ == this || i->owner_ == nullptr)
		{
			if (RENDER* r = dynamic_cast<RENDER*>(i))
			{
				delete r->render_.model_;
			}
			else if (TRANSFORM* t = dynamic_cast<TRANSFORM*>(i))
			{
				delete t->trans_;
			}
			delete i;
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

	double halfw = width / 2.0;
	double halfh = height / 2.0;
	double end = centeryon.getZ();
	// orthogonal cs to screen
	transformation towindow = scale(point{halfw/SCREEN_DIM, -halfh/SCREEN_DIM, 1});

	std::vector<plane> planes =
	{
		plane({-100, 0, 0}, {1, 0, 0}), // left
		plane({100, 0, 0}, {-1, 0, 0}), // right
		plane({0, -100, 0}, {0, 1, 0}), // down
		plane({0, 100, 0}, {0, -1, 0}), // up
		plane({0, 0, 0}, {0, 0, 1}), // front
		plane({0, 0, end}, {0, 0, -1}) // back
	};
	transformation Ktrans;
	transformation CTMP;
	bool cam_proj = false;

	for (INSTRUCTION* i : instructions_)
	{
		assert(false == ctfs.empty()); // ctfs may never be empty
		while (i->stack_ < ctfs.size() - 1)
		{
			ctfs.pop();
		}
		while (i->stack_ >= ctfs.size())
		{
			transformation cpy = ctfs.top();
			ctfs.push(cpy);
		}
		transformation& ctf = ctfs.top();
		if (RENDER* r = dynamic_cast<RENDER*>(i))
        {
			// transform in order of ctf
			r->render_.model_->transform(ctf);
			shapes.push_back(r->render_);
		}
		else if (TRANSFORM* t = dynamic_cast<TRANSFORM*>(i))
		{
			ctf = ctf.matmul(*t->trans_);
		}
		else if (CAMERA_INST* cam = dynamic_cast<CAMERA_INST*>(i))
		{
			auto dist = [](double val)
			{
				return sqrt(1 + val*val);
			};
			planes = {
				plane({0, 0, 0}, {1/dist(cam->left_), 0, std::abs(cam->left_)/dist(cam->left_)}),
				plane({0, 0, 0}, {-1/dist(cam->right_), 0, std::abs(cam->right_)/dist(cam->right_)}),
				plane({0, 0, 0}, {0, 1/dist(cam->down_), std::abs(cam->down_)/dist(cam->down_)}),
				plane({0, 0, 0}, {0, -1/dist(cam->top_), std::abs(cam->top_)/dist(cam->top_)}),
				plane({0, 0, cam->front_}, {0, 0, 1}),
				plane({0, 0, cam->back_}, {0, 0, -1})
			};
			for (plane& p : planes)
			{
				ctf.mul(p.pt_.x, p.pt_.y, p.pt_.z);
				ctf.mul(p.norm_.x, p.norm_.y, p.norm_.z);
			}

			// insert camera transformation
			Ktrans = camera_transform(1);
			CTMP = ctf.inverse();
			// everything's projected to z=1 plane
			double planewidthx = (cam->right_ - cam->left_)/2;
			double planewidthy = (cam->top_ - cam->down_)/2;
			towindow = scale(point{halfw/planewidthx, -halfh/planewidthy, 1});
			cam_proj = true;
		}
	}

    towindow = translate(point{centeryon.getX(), centeryon.getY()}).matmul(towindow);
	for (shape_render& rends : shapes)
	{
		if (!rends.model_->clip_planes(planes))
		{
			if (cam_proj)
			{
				rends.model_->transform(CTMP);
				// cache model's z coord
				std::vector<point> zs;
				for (size_t i = 0, n = rends.model_->n_vertices();
					 i < n; i++)
				{
					point p = rends.model_->get_v(i);
					zs.push_back({0, 0, p.z * p.z, 0});
				}
				rends.model_->transform(Ktrans);
				// add z coord to z
				rends.model_->add_point(zs);
			}
			rends.model_->transform(towindow);
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
		switch(c)
		{
			case '#': // comment
				// skip until next new line
				exhaust_until(s, lhqueue, '\n');
				break;
			case '{':
				lexeme.push_back(c);
				token = PUSH_STACK;
				break;
			case '}':
				lexeme.push_back(c);
				token = POP_STACK;
				break;
			case 's': // scale or surface
				if (this->lookahead(s, lhqueue, "cale", false))
				{
					token = SCALE;
					// look at format <x>,<y>,<z>
					lexeme = this->delimited(s, lhqueue, {',', ' '}, 2);
				}
				else if (this->lookahead(s, lhqueue, "urface"))
				{
					token = SURFACE;
					// expect format: (<r>,<g>,<b>)
					lexeme = this->find_first_of(s, lhqueue, {')'});
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
					lexeme = this->delimited(s, lhqueue, {',', ' '}, 2);
				}
				break;
			case 'l': // line
				if (this->lookahead(s, lhqueue, "ine"))
				{
					token = LINE;
					// look at format (<x>,<y>,<h>)(<x>,<y>,<h>)
					// OR (<x>,<y>,<h>,<r>,<g>,<b>)(<x>,<y>,<h>,<r>,<g>,<b>)
					for (size_t i = 0; i < 2; i++) {
						lexeme += this->find_first_of(s, lhqueue, {')'});
					}
				}
				break;
			case 'p': // polygon
				if (this->lookahead(s, lhqueue, "olygon")) {
					token = POLYGON;
					// look at format (<x>,<y>,<h>)(<x>,<y>,<h>),(<x>,<y>,<h>)
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
				else if (this->lookahead(s, lhqueue, "illed")) {
					token = FILL;
					lexeme = "filled";
				}
				break;
			case 'c': // camera
				if (this->lookahead(s, lhqueue, "amera"))
				{
					token = CAMERA;
					// expect format <xlow> <ylow> <xhigh> <yhigh> <hither> <yon>
					lexeme = this->delimited(s, lhqueue, whitespace, 5);
				}
				break;
			case 'o': // obj
				if (this->lookahead(s, lhqueue, "bj"))
				{
					token = OBJ;
					// expect format "filename"
					for (size_t i = 0; i < 2; i++) {
						lexeme += this->find_first_of(s, lhqueue, {'"'});
					}
				}
				break;
			case 'a': // ambient
				if (this->lookahead(s, lhqueue, "mbient"))
				{
					token = AMBIENT;
					// expect format: (<r>,<g>,<b>)
					lexeme = this->find_first_of(s, lhqueue, {')'});
				}
				break;
			case 'd': // depth
				if (this->lookahead(s, lhqueue, "epth"))
				{
					token = DEPTH;
					// expect format: <near> <far> (<r>,<g>,<b>)
					lexeme = this->delimited(s, lhqueue, whitespace, 2);
					lexeme += this->find_first_of(s, lhqueue, {')'});
				}
				break;
			default: // ignore
				break;
		}
		if (token != INVALID)
        {
			lextok_.push({lexeme, token});
		}
    }
}

point simp_reader::to_point (std::string pts, std::string delim,
		std::unordered_set<char> ignore,
		color def_color) const
{
// pts should be in format #,#,# or #,#,#,#,#,#
	std::vector<std::string> vals = this->split(pts, delim);
	std::vector<double> sfactors;
	for (std::string sval : vals)
	{
		std::string propers = this->trim(sval, ignore);
		if (!propers.empty())
		{
			sfactors.push_back(std::atof(propers.data())); // doesn't check for non-numerics
		}
	}
	if (sfactors.size() != 3 && sfactors.size() != 6)
	{
		throw std::exception(); // todo: better exception: invalid syntax
	}
	point p{sfactors[0], sfactors[1], sfactors[2]};
	if (sfactors.size() > 3)
	{
		p.basecolor = color((uint8_t) 255 * sfactors[3], (uint8_t) 255 * sfactors[4], (uint8_t) 255 * sfactors[5]);
	}
	else
	{
		p.basecolor = def_color;
	}
	return p;
}

color_grad simp_reader::to_rgb(std::string cs, std::string delim,
	 std::unordered_set<char> ignore) const
{
	std::vector<std::string> rgb = this->split(cs, " ");
	std::vector<double> sfactors;
	for (std::string sval : rgb)
	{
		std::string propers = this->trim(sval, ignore);
		if (!propers.empty())
		{
			sfactors.push_back(std::atof(propers.data())); // doesn't check for non-numerics
		}
	}
	if (sfactors.size() != 3)
	{
		throw std::exception(); // todo: better exception: invalid syntax
	}
	return color_grad(sfactors[0], sfactors[1], sfactors[2]);
}

static std::string remove_comma (std::string s, const std::unordered_set<char>& whitespace)
{
	bool whitespaced = false;
	std::string res = "";
	for (char c : s)
	{
		if (whitespace.end() != whitespace.find(c) || c == ',')
		{
			whitespaced = true;
		}
		else
		{
			if (whitespaced)
			{
				res.push_back(' ');
			}
			res.push_back(c);
			whitespaced = false;
		}
	}
	return res;
}

// instructions are either models or transformations
void simp_reader::parse (DRAW drawer)
{
	std::unordered_set<char> whitespace = whiteset();
	std::unordered_set<char> filter = whitespace;
	filter.insert('(');
	filter.insert(')');
	filter.insert('"');
	size_t pushcount = 0;
	bool polyfill = true;

	// phong shade with depth
	DRAW drawerwrapper =
	[drawer] (int x, int y, int z, unsigned c)
	{
		drawer(x, y, z, c);
	};
	std::shared_ptr<ishaper> liner = std::shared_ptr<ishaper>(new dda_liner(drawerwrapper));
	if (nullptr == goner_)
	{
		goner_ = std::shared_ptr<ishaper>(new convex_filler(drawerwrapper));
	}
	bool is_fill = true;

	while (false == lextok_.empty())
	{
		INSTRUCTION* inst = nullptr;
		LEX_TOK lt = lextok_.front();
		std::string lexeme = lt.first;
		lexeme = remove_comma(lexeme, whitespace);
		SIMP_TOK token = (SIMP_TOK) lt.second;
        lextok_.pop();
		switch (token)
        {
			case PUSH_STACK:
				pushcount++;
				inst = new INSTRUCTION(pushcount);
				break;
			case POP_STACK:
				pushcount--;
				inst = new INSTRUCTION(pushcount);
				break;
			case SCALE:
			{
				point pt = to_point(lexeme, " ", whitespace, surface_);
				inst = new TRANSFORM(new scale(pt), pushcount);
			}
				break;
			case ROTATE: // lexeme should be in format [X Y Z],#
			{
				std::vector<std::string> vals = this->split(lexeme, " ");
				if (vals.size() != 2)
				{
					throw std::exception(); // todo: better exception: invalid syntax
				}
				char axis = this->trim(vals[0], whitespace)[0];
				double degrees = std::atof(this->trim(vals[1], whitespace).data()); // doesn't check for non-numerics
				inst = new TRANSFORM(new rotate(axis, degrees), pushcount);
			}
				break;
			case TRANSLATE:
			{
				point pt = to_point(lexeme, " ", whitespace, surface_);
				inst = new TRANSFORM(new translate(pt), pushcount);
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
					to_point(pts[0], " ", filter, surface_),
					to_point(pts[1], " ", filter, surface_));
				inst = new RENDER(line, liner, pushcount);
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
                    to_point(pts[0], " ", filter, surface_),
                    to_point(pts[1], " ", filter, surface_),
                    to_point(pts[2], " ", filter, surface_)
                };
                poly_model* poly = new poly_model(respts);
				inst = new RENDER(poly, goner_, pushcount);
			}
				break;
			case FILE:
			{
				std::string f = this->trim(lexeme, filter);
				f += ".simp";
				// read and parse f
				simp_reader* reader = new simp_reader(directory_ + "/" + f,
					drawerwrapper, ambient_, surface_, goner_);
				reader->get_instructions(
				[this, pushcount](INSTRUCTION* i) {
					i->stack_ += pushcount;
					instructions_.push_back(i);
				});
				subreaders_.push_back(reader);
			}
				break;
			case WIRE:
				is_fill = false;
				goner_ = std::shared_ptr<ishaper>(new convex_wirer(drawerwrapper));
				break;
			case FILL:
				is_fill = true;
				goner_ = std::shared_ptr<ishaper>(new convex_filler(drawerwrapper));
				break;
			case CAMERA:
			{
				// expect format <xlow> <ylow> <xhigh> <yhigh> <hither> <yon>
				std::vector<std::string> vals = this->split(lexeme, " ");
				if (vals.size() != 6)
				{
					throw std::exception(); // todo: better exception: invalid syntax
				}
				double xlow = std::atof(this->trim(vals[0], whitespace).data());
				double ylow = std::atof(this->trim(vals[1], whitespace).data());
				double xhigh = std::atof(this->trim(vals[2], whitespace).data());
				double yhigh = std::atof(this->trim(vals[3], whitespace).data());
				double hither = std::atof(this->trim(vals[4], whitespace).data());
				double yon = std::atof(this->trim(vals[5], whitespace).data());

				inst = new CAMERA_INST(xlow, xhigh, ylow, yhigh, hither, yon, pushcount);
			}
				break;
			case OBJ:
			{
				std::vector<poly_model*> objs;
				std::string f = this->trim(lexeme, filter);
                f += ".obj";
                obj_reader reader(directory_ + "/" + f, surface_);
                reader.get_objects(objs);
                if (!objs.empty())
                {
                    auto it = objs.begin();
                    inst = new RENDER(*it, goner_, pushcount);
                    it++;
                    for (auto et = objs.end(); it != et; it++)
                    {
                        instructions_.push_back(new RENDER(*it, goner_, pushcount));
                    }
                }
			}
				break;
			case AMBIENT:
			{
				// expect format: (<r>,<g>,<b>)
				ambient_ = to_rgb(lexeme, " ", filter);
			}
				break;
			case DEPTH:
			{
				// expect format: <near> <far> (<r>,<g>,<b>)
				std::vector<std::string> nfc = this->split(lexeme, "(");
				if (nfc.size() != 2)
				{
					throw std::exception(); // todo: better exception: invalid syntax
				}
				std::vector<std::string> vals = this->split(nfc[0], " ");
				if (vals.size() != 2)
				{
					throw std::exception(); // todo: better exception: invalid syntax
				}
				double near = std::atof(this->trim(vals[0], whitespace).data());
				double far = std::atof(this->trim(vals[1], whitespace).data());
				color_grad cg = to_rgb(nfc[1], " ", filter);
				color depth(255*cg.r, 255*cg.g, 255*cg.b);
				drawerwrapper =
				[near, far, depth, drawer] (int x, int y, int z, unsigned c)
				{
					assert(far >= near);
					if (z < near || far == near)
					{
						drawer(x, y, z, c);
					}
					else if (z > far)
					{
						drawer(x, y, z, (unsigned)depth);
					}
					else
					{
						color_grad cc(c);
						double fog = (far - z) / (far - near);
						double invfog = 1 - fog;
						cc.r = fog * cc.r + invfog * depth.r;
						cc.g = fog * cc.g + invfog * depth.g;
						cc.b = fog * cc.b + invfog * depth.b;
						drawer(x, y, z, (unsigned)cc);
					}
				};
				if (is_fill)
				{
					goner_ = std::shared_ptr<ishaper>(new convex_filler(drawerwrapper));
				}
				else
				{
					goner_ = std::shared_ptr<ishaper>(new convex_wirer(drawerwrapper));
				}
			}
				break;
			case SURFACE:
			{
				// expect format: (<r>,<g>,<b>)
				color_grad cg = to_rgb(lexeme, " ", filter);
				surface_ = color(255*cg.r, 255*cg.g, 255*cg.b);
			}
				break;
			default: // ignore INVALID
				break;
		}
		if (inst)
		{
			if (RENDER* r = dynamic_cast<RENDER*>(inst))
			{
				// gaurad shade with ambience
				r->render_.model_->colorpoints(
				[this](color& c)
				{
					c.r *= ambient_.r;
					c.g *= ambient_.g;
					c.b *= ambient_.b;
				});
			}
			instructions_.push_back(inst);
		}
	}
}

}

#endif
