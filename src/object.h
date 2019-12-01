#ifndef OBJECT_H
#define OBJECT_H

#include <SFML/Graphics.hpp>
#include "constants.h"
#include "structures.h"

class Object
{
public:
	bool rotatable = false;

	unsigned short id;
	unsigned short x;
	unsigned short y;
	unsigned short direction = 0;
	sf::Sprite sprite;
	std::vector<sf::Texture*> textures;

	void rotate(bool clockwise);
};

class Beamer : public Object
{
public:
	Beamer(Color col) : color(col)
	{
		rotatable = true;
	}

	Color color;
	Laser laser;
};

class Dot : public Object
{
public:
	Dot(Color col) : color(col), actualColor(false, false, false)
	{
		rotatable = false;
		sprite.setColor(color.convertToColor());
	}

	Color color;
	Color actualColor;
	bool state = false;

	void updateState();
};

class Mirror : public Object
{
public:
	Mirror()
	{
		rotatable = true;
	}
};

class Bender : public Object
{
public:
	Bender()
	{
		rotatable = true;
	}
};

#endif
