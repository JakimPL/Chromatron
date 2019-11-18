#ifndef OBJECT_H
#define OBJECT_H

#define OBJ_BEAMER 1
#define OBJ_DOT 2
#define OBJ_MIRROR 3
#define OBJ_BENDER 4

#include <SFML/Graphics.hpp>
#include "game.h"
#include "structures.h"

class Object
{
public:
	unsigned short id;
	unsigned short x;
	unsigned short y;
	Game* game;
	sf::Sprite sprite;

	void initialize(Game* gm, unsigned short xx, unsigned short yy);
};

class Beamer : public Object
{
public:
	Beamer(Game* gm, unsigned short xx, unsigned short yy, Color col, unsigned short dir) : color(col), direction(dir)
	{
		id = OBJ_BEAMER;
		initialize(gm, xx, yy);
	}

	Color color;
	unsigned short direction;
};

class Dot : public Object
{
public:
	Dot(Game* gm, unsigned short xx, unsigned short yy, Color col) : color(col)
	{
		id = OBJ_DOT;
		initialize(gm, xx, yy);
	}

	Color color;
};

class Mirror : public Object
{
public:
	Mirror(Game* gm, unsigned short xx, unsigned short yy, unsigned short dir) : direction(dir)
	{
		id = OBJ_MIRROR;
		initialize(gm, xx, yy);
	}

	unsigned short direction;
};

class Bender : public Object
{
public:
	Bender(Game* gm, unsigned short xx, unsigned short yy, unsigned short dir) : direction(dir)
	{
		id = OBJ_BENDER;
		initialize(gm, xx, yy);
	}

	unsigned short direction;
};

#endif
