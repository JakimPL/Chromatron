#ifndef OBJECT_H
#define OBJECT_H

#define OBJ_BEAMER 1
#define OBJ_DOT 2
#define OBJ_MIRROR 3
#define OBJ_BENDER 4

#include <SFML/Graphics.hpp>
#include "structures.h"

class Object
{
public:
	unsigned short id;
	unsigned short x;
	unsigned short y;
	//sf::Sprite sprite;
};

class Beamer : public Object
{
public:
	Beamer(unsigned short xx, unsigned short yy, Color col, unsigned short dir) : color(col), direction(dir)
	{
		id = OBJ_BEAMER;
		x = xx;
		y = yy;
	}

	Color color;
	unsigned short direction;
};

class Dot : public Object
{
public:
	Dot(unsigned short xx, unsigned short yy, Color col) : color(col)
	{
		id = OBJ_DOT;
		x = xx;
		y = yy;
	}

	Color color;
};

class Mirror : public Object
{
public:
	Mirror(unsigned short xx, unsigned short yy, unsigned short dir) : direction(dir)
	{
		id = OBJ_MIRROR;
		x = xx;
		y = yy;
	}

	unsigned short direction;
};

class Bender : public Object
{
public:
	Bender(unsigned short xx, unsigned short yy, unsigned short dir) : direction(dir)
	{
		id = OBJ_BENDER;
		x = xx;
		y = yy;
	}

	unsigned short direction;
};

#endif
