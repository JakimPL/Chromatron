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
	sf::Sprite sprite;
};

class Beamer : public Object
{
	void calculateLaser();
public:
	Beamer(Color col) : color(col) {}

	Color color;
	unsigned short direction;
	Laser laser;
};

class Dot : public Object
{
public:
	Dot(Color col) : color(col) {}

	Color color;
};

class Mirror : public Object
{
public:
	unsigned short direction;
};

class Bender : public Object
{
public:
	unsigned short direction;
};

#endif
