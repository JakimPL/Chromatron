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
	virtual std::vector<RayGenElement> interaction(RayGen &rayGen);
};

class Beamer : public Object
{
public:
	Beamer(Color col = COL_BLACK_TUPLE);

	Laser laser;

	std::vector<RayGenElement> interaction(RayGen &rayGen) override;
};

class Dot : public Object
{
public:
	Dot(Color col = COL_BLACK_TUPLE);

	Color actualColor;
	bool state = false;

	void updateState();

	std::vector<RayGenElement> interaction(RayGen &rayGen) override;
};

class Mirror : public Object
{
public:
	Mirror();

	std::vector<RayGenElement> interaction(RayGen &rayGen) override;
};

class Bender : public Object
{
public:
	Bender();

	std::vector<RayGenElement> interaction(RayGen &rayGen) override;
};

class Splitter : public Object
{
public:
	Splitter();

	std::vector<RayGenElement> interaction(RayGen &rayGen) override;
};

class Conduit : public Object
{
public:
	Conduit();

	std::vector<RayGenElement> interaction(RayGen &rayGen) override;
};

class Filter : public Object
{
public:
	Filter(Color col = COL_BLACK_TUPLE);

	std::vector<RayGenElement> interaction(RayGen &rayGen) override;
};

class Prism : public Object
{
public:
	Prism();

	std::vector<RayGenElement> interaction(RayGen &rayGen) override;
};

class Doppler : public Object
{
public:
	Doppler();

	std::vector<RayGenElement> interaction(RayGen &rayGen) override;
};

class Tangler : public Object
{
public:
	Tangler();

	std::vector<RayGenElement> interaction(RayGen &rayGen) override;
};

#endif