#ifndef OBJECT_H
#define OBJECT_H

#include <SFML/Graphics.hpp>
#include "constants.h"
#include "structures.h"

class Object
{
public:
	unsigned short id;
	unsigned short x;
	unsigned short y;
	unsigned short direction = 0;
	sf::Sprite sprite;
	std::vector<sf::Texture*> textures;
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
	Dot(Color col) : color(col), actualColor({0, 0, 0})
	{
		sprite.setColor(color.convertToColor());
	}

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
