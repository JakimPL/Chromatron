#ifndef OBJECT_H
#define OBJECT_H

#include "structures.h"

class Object
{
public:
	unsigned short x;
	unsigned short y;
};

class Beamer : public Object
{
public:
	Beamer(unsigned short xx, unsigned short yy, Color col, unsigned short dir) : color(col), direction(dir)
	{
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
		x = xx;
		y = yy;
	}

	unsigned short direction;
};

#endif
