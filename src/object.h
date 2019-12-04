#ifndef OBJECT_H
#define OBJECT_H

#include <SFML/Graphics.hpp>
#include "constants.h"
#include "structures.h"

class Object
{
public:
	Object(Color col = COL_BLACK_TUPLE);
	bool rotatable = false;
	bool movable = false;
	bool colorable = false;
	Objects id;
	Directions direction = DIR_NORTH;
	sf::Sprite sprite;
	Color color;

	struct Position {
	private:
		short x = -1;
		short y = -1;
	public:
		static Position createPosition(short xx, short yy);
		static Position createPosition(sf::Vector2f vector);

		bool operator<(const Position &pos) const;
		bool operator==(const Position &pos) const;
		bool operator!=(const Position &pos) const;
		operator sf::Vector2f();

		short getX();
		short getY();
		bool isNull();
		void setNull();
		void moveInDirection(unsigned short dir, int length);
		void setPosition(short xx, short yy);
		void setPosition(Position position);
	} position;

	std::vector<sf::Texture*> textures;

	void rotate(bool clockwise, bool force = false);
	void updateSprite();
};

class Beamer : public Object
{
public:
	Beamer(Color col = COL_BLACK_TUPLE);

	Laser laser;
};

class Dot : public Object
{
public:
	Dot(Color col = COL_BLACK_TUPLE);

	Color actualColor;
	bool state = false;

	void updateState();
	void setSpriteColor();
};

class Mirror : public Object
{
public:
	Mirror();

};

class Bender : public Object
{
public:
	Bender();
};

#endif
