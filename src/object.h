#ifndef OBJECT_H
#define OBJECT_H

#include <SFML/Graphics.hpp>
#include "constants.h"
#include "structures.h"

class Object
{
public:
	bool rotatable = false;
	bool movable = false;
	unsigned short id;
	unsigned short direction = 0;
	sf::Sprite sprite;
	std::vector<sf::Texture*> textures;

	struct Position {
	private:
		short x;
		short y;
	public:
		bool operator<(const Position &pos) const;
		bool operator==(const Position &pos);
		bool operator!=(const Position &pos);
		operator sf::Vector2f();
		static Position createPosition(short xx, short yy);
		static Position createPosition(sf::Vector2f vector);
		short getX();
		short getY();
		void moveInDirection(unsigned short dir, int length);
		void setPosition(short xx, short yy);
	} position;

	void rotate(bool clockwise);
	void updateSprite();
};

class Beamer : public Object
{
public:
	Beamer(Color col) : color(col)
	{
		rotatable = false;
		movable = false;
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
		movable = false;
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
		movable = true;
	}
};

class Bender : public Object
{
public:
	Bender()
	{
		rotatable = true;
		movable = true;
	}
};

#endif
