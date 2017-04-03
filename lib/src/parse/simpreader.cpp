//
// Created by Mingkai Chen on 2017-02-16.
//

#include "lib/include/parse/simpreader.h"
#include "lib/include/camera.h"

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
	SURFACE,
	LIGHT,
	FLAT,
	GOURAUD,
	PHONG
};

simp_reader::simp_reader (std::string path)
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

	double end = centeryon.getZ();
	point centerhither = centeryon;
	centerhither.z = 0;
	// orthogonal cs to screen
	camera* cam = new camera(end, {width/2, height/2}, centerhither);
	light sources;
	SHADING_METHOD shad = FLAT_SHAD;

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
			r->render_.sources_ = sources;
			r->render_.shad_ = shad;
			shapes.push_back(r->render_);
		}
		else if (TRANSFORM* t = dynamic_cast<TRANSFORM*>(i))
		{
			ctf = ctf.matmul(*t->trans_);
		}
		else if (CAMERA_INST* camInst = dynamic_cast<CAMERA_INST*>(i))
		{
			delete cam;
			projective_cam* proj = new projective_cam(
				camInst->left_, camInst->right_,
				camInst->top_, camInst->down_,
				camInst->front_, camInst->back_,
				{width/2, height/2}, centerhither);
			cam = proj;
			proj->transform(ctf);
		}
		else if (AMBIENT_INST* amb = dynamic_cast<AMBIENT_INST*>(i))
		{
			sources.ambient_ = amb->cg;
		}
		else if (LIGHT_INST* lsrc = dynamic_cast<LIGHT_INST*>(i))
		{
			double x = 0, y = 0, z = 0;
			ctf.mul(x, y, z);
			sources.srcs_.push_back(std::make_shared<light::bulb>(lsrc->A, lsrc->B, normal(x, y, z), lsrc->cg));
		}
		else if (SHADING_INST* shadinst = dynamic_cast<SHADING_INST*>(i))
		{
			shad = shadinst->shad_;
		}
	}

	for (shape_render& rends : shapes)
	{
		cam->render(rends);
	}
	delete cam;
}

// implements a pseudo tokenizer that implies partial information on grammar (otherwise we'll need regex for numerics)
void simp_reader::tokenize (std::istream& s)
{
	std::unordered_set<char> whitespace = whiteset();
	std::list<char> lhqueue; // look ahead queue
	while (s.good())
	{
		char c;
		if (lhqueue.empty())
		{
			c = s.get();
		}
		else
		{
			c = lhqueue.front();
			lhqueue.pop_front();
		}
		std::string lexeme = "";
		SIMP_TOK token = INVALID;
		switch(c)
		{
			case '#': // comment
				// skip until next new line
				exhaust_until(s, lhqueue, {'\n'});
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
					lexeme = exhaust_until(s, lhqueue, {'#', '\n'});
				}
				else if (this->lookahead(s, lhqueue, "urface"))
				{
					token = SURFACE;
					// expect format: (<r>,<g>,<b>) ks p
					lexeme = exhaust_until(s, lhqueue, {'#', '\n'});
				}
				break;
			case 'r': // rotate
				if (this->lookahead(s, lhqueue, "otate"))
				{
					token = ROTATE;
					// look at format <axis> <angle>
					lexeme = exhaust_until(s, lhqueue, {'#', '\n'});
				}
				break;
			case 't': // translate
				if (this->lookahead(s, lhqueue, "ranslate"))
				{
					token = TRANSLATE;
					// look at format <x>,<y>,<z>
					lexeme = exhaust_until(s, lhqueue, {'#', '\n'});
				}
				break;
			case 'l': // line or light
				if (this->lookahead(s, lhqueue, "ine", false))
				{
					token = LINE;
					// look at format (<x>,<y>,<h>)(<x>,<y>,<h>)
					// OR (<x>,<y>,<h>,<r>,<g>,<b>)(<x>,<y>,<h>,<r>,<g>,<b>)
					lexeme = exhaust_until(s, lhqueue, {'#', '\n'});
				}
				else if (this->lookahead(s, lhqueue, "ight"))
				{
					token = LIGHT;
					// look at format <r> <g> <b> <A> <B>
					lexeme = exhaust_until(s, lhqueue, {'#', '\n'});
				}
				break;
			case 'p': // polygon or phong
				if (this->lookahead(s, lhqueue, "olygon", false))
				{
					token = POLYGON;
					// look at format (<x>,<y>,<h>)(<x>,<y>,<h>),(<x>,<y>,<h>)
					lexeme = exhaust_until(s, lhqueue, {'#', '\n'});
				}
				else if (this->lookahead(s, lhqueue, "ong"))
				{
					token = PHONG;
					lexeme = "phong";
				}
				break;
			case 'w': // wire
				if (this->lookahead(s, lhqueue, "ire"))
				{
					token = WIRE;
					lexeme = "wire";
				}
				break;
			case 'f': // file or fill
				if (this->lookahead(s, lhqueue, "ile", false))
				{
					token = FILE;
					// look at format "word"
					lexeme = exhaust_until(s, lhqueue, {'#', '\n'});
				}
				// this look ahead will jump to next space or end of match if match is wrong
				else if (this->lookahead(s, lhqueue, "illed", false))
				{
					token = FILL;
					lexeme = "filled";
				}
				else if (this->lookahead(s, lhqueue, "lat"))
				{
					token = FLAT;
					lexeme = "flat";
				}
				break;
			case 'c': // camera
				if (this->lookahead(s, lhqueue, "amera"))
				{
					token = CAMERA;
					// expect format <xlow> <ylow> <xhigh> <yhigh> <hither> <yon>
					lexeme = exhaust_until(s, lhqueue, {'#', '\n'});
				}
				break;
			case 'o': // obj
				if (this->lookahead(s, lhqueue, "bj"))
				{
					token = OBJ;
					// expect format "filename"
					lexeme = exhaust_until(s, lhqueue, {'#', '\n'});
				}
				break;
			case 'a': // ambient
				if (this->lookahead(s, lhqueue, "mbient"))
				{
					token = AMBIENT;
					// expect format: (<r>,<g>,<b>)
					lexeme = exhaust_until(s, lhqueue, {'#', '\n'});
				}
				break;
			case 'd': // depth
				if (this->lookahead(s, lhqueue, "epth"))
				{
					token = DEPTH;
					// expect format: <near> <far> (<r>,<g>,<b>)
					lexeme = exhaust_until(s, lhqueue, {'#', '\n'});
				}
				break;
			case 'g':
				if (this->lookahead(s, lhqueue, "ouraud"))
				{
					token = GOURAUD;
					lexeme = "gouraud";
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

color_grad simp_reader::to_rgb (std::string cs, std::string delim,
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
                poly->lerp_norm();
                poly->kd_ = kd_;
				poly->ks_ = ks_;
				poly->p_ = p_;
				inst = new RENDER(poly, goner_, pushcount);
			}
				break;
			case FILE:
			{
				std::string f = this->trim(lexeme, filter);
				f += ".simp";
				// read and parse f
				simp_reader* reader = new simp_reader(directory_ + "/" + f);
				reader->surface_ = surface_;
				reader->goner_ = goner_;
				reader->kd_ = kd_;
				reader->ks_ = ks_;
				reader->p_ = p_;

				reader->parse(drawerwrapper);
				reader->get_instructions(
				[this, pushcount](INSTRUCTION* i) {
					i->stack_ += pushcount;
					instructions_.push_back(i);
				});
				subreaders_.push_back(reader);

				surface_ = reader->surface_;
				goner_ = reader->goner_;
				kd_ = reader->kd_;
				ks_ = reader->ks_;
				p_ = reader->p_;
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
                obj_reader reader(directory_ + "/" + f);
                reader.basecolor_ = surface_;
                reader.parse([](int,int,int,unsigned) {});
                reader.get_objects(objs);
                if (!objs.empty())
                {
                    auto it = objs.begin();
					poly_model* poly = *it;
					poly->lerp_norm();
					poly->kd_ = kd_;
					poly->ks_ = ks_;
					poly->p_ = p_;
                    inst = new RENDER(poly, goner_, pushcount);
                    it++;
                    for (auto et = objs.end(); it != et; it++)
                    {
                    	poly = *it;
						poly->lerp_norm();
						poly->kd_ = kd_;
						poly->ks_ = ks_;
						poly->p_ = p_;
                        instructions_.push_back(new RENDER(poly, goner_, pushcount));
                    }
                }
				surface_ = reader.basecolor_;
			}
				break;
			case AMBIENT:
			{
				// expect format: (<r>,<g>,<b>)
				inst = new AMBIENT_INST(to_rgb(lexeme, " ", filter), pushcount);
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
				// expect format: (<r>,<g>,<b>) ks p
				std::vector<std::string> nfc = this->split(lexeme, ")");
				size_t nnfc = nfc.size();
				if (nnfc > 2)
				{
					throw std::exception(); // todo: better exception: invalid syntax
				}
				kd_ = to_rgb(nfc[0], " ", filter);
				surface_ = color(255*kd_.r, 255*kd_.g, 255*kd_.b);
				if (nnfc > 1)
				{
					std::vector<std::string> vals = this->split(nfc[1], " ");
					if (vals.size() != 2)
					{
						throw std::exception(); // todo: better exception: invalid syntax
					}
					ks_ = std::atof(this->trim(vals[0], whitespace).data());
					p_ = std::atof(this->trim(vals[1], whitespace).data());
				}
			}
				break;
			case LIGHT:
			{
				// expect format <r> <g> <b> <A> <B>
				std::vector<std::string> vals = this->split(lexeme, " ");
				if (vals.size() != 5)
				{
					throw std::exception(); // todo: better exception: invalid syntax
				}
				double r = std::atof(this->trim(vals[0], filter).data());
				double g = std::atof(this->trim(vals[1], filter).data());
				double b = std::atof(this->trim(vals[2], filter).data());

				double A = std::atof(this->trim(vals[3], filter).data());
				double B = std::atof(this->trim(vals[4], filter).data());

				inst = new LIGHT_INST(color_grad(r, g, b), A, B, pushcount);
			}
				break;
			case FLAT: // todo: perform shading operations
			{
				inst = new SHADING_INST(FLAT_SHAD);
			}
				break;
			case GOURAUD:
			{
				inst = new SHADING_INST(GOURAUD_SHAD);
			}
				break;
			case PHONG:
			{
				inst = new SHADING_INST(PHONG_SHAD);
			}
				break;
			default: // ignore INVALID
				break;
		}
		if (inst)
		{
			instructions_.push_back(inst);
		}
	}
}

}

#endif
