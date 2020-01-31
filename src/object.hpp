#ifndef OBJECT_H
#define OBJECT_H

#include <SFML/Graphics.hpp>
#include "constants.hpp"
#include "color.hpp"
#include "position.hpp"
#include "raygen.hpp"

class Object
{
protected:
	Object(Color col = COL_BLACK_TUPLE);
public:
	bool inStack = false;
	bool rotatable = false;
	bool movable = false;
	bool colorable = false;
	ObjectID id;
	DirectionID direction = DIR_NORTH;

	std::vector<sf::Texture*> textures;
	sf::Sprite baseSprite;
	sf::Sprite sprite;
	Color color;
	Position position;

	virtual void rotate(bool clockwise, bool force = false);
	virtual void setSpriteColor();
	virtual void updateSprite();
	//virtual std::vector<RayGen> interaction(RayGen &rayGen, bool &stop, bool &end);
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

class Splitter : public Object
{
public:
	Splitter();
	//std::vector<RayGen> interaction(RayGen &rayGen, bool &stop, bool &end) override;
};

class Conduit : public Object
{
public:
	Conduit();
};

class Filter : public Object
{
public:
	Filter(Color col = COL_BLACK_TUPLE);
};

class Prism : public Object
{
public:
	Prism();
};

class Doppler : public Object
{
public:
	Doppler();
};

class Tangler : public Object
{
public:
	Tangler();
};

#endif