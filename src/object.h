#ifndef OBJECT_H
#define OBJECT_H

#define OBJ_BEAMER 1
#define OBJ_DOT 2
#define OBJ_MIRROR 3
#define OBJ_BENDER 4
#define OBJ_DOTF 5

#include <SFML/Graphics.hpp>
#include "structures.h"

class Object
{
public:
	unsigned short id;
	unsigned short x;
	unsigned short y;
	unsigned short direction = 0;
	sf::Sprite sprite;
};

class Beamer : public Object
{
public:
	Beamer(Color col) : color(col) {}

	Color color;
	Laser laser;
};

class Dot : public Object
{
public:
	Dot(Color col) : color(col), actualColor({0, 0, 0}) {}

	Color color;
	Color actualColor;
	bool state = false;

	void updateState();
};

class Mirror : public Object
{
};

class Bender : public Object
{
};

#endif
