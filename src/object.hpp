#ifndef OBJECT_H
#define OBJECT_H

#include <SFML/Graphics.hpp>
#include "constants.hpp"
#include "color.hpp"
#include "position.hpp"
#include "ray.hpp"

class Game;

class Object
{
protected:
	Object(Color col = COL_BLACK_TUPLE);
public:
	bool inStack = false;
	bool rotatable = false;
	bool movable = false;
	bool colorable = false;

	Game *game;
	ObjectID id;
	DirectionID direction = DIR_NORTH;
	Position position;
	Color color;

	std::vector<sf::Texture*> textures;
	sf::Sprite baseSprite;
	sf::Sprite sprite;

	virtual ~Object();
	virtual void rotate(bool clockwise, bool force = false);
	virtual void setObject(Game *gam, Position pos, ObjectID obID, DirectionID dir = DIR_NORTH, bool inSt = false, bool stackObject = false);
	virtual void setSprite(sf::Sprite &sprite, size_t textureIndex);
	virtual void setSpriteColor();
	virtual void updateSprite();
	virtual void writeGeneralData(std::ofstream &file);
	virtual void writeObject(std::ofstream &file);
	virtual RayGenList interaction(Ray &ray, RayGen &rayGen);

	typedef std::map<ObjectID, Object*(*)()> TypeMap;
	static TypeMap typeMap;

	template<typename T> static Object* createObject();
	static void createObjectMap();
};

class Beamer : public Object
{
public:
	Beamer(Color col = COL_BLACK_TUPLE);

	Laser laser;

	void writeObject(std::ofstream &file) override;
	RayGenList interaction(Ray &ray, RayGen &rayGen) override;
};

class Dot : public Object
{
public:
	Dot(Color col = COL_BLACK_TUPLE);

	Color actualColor;
	bool state = false;

	void updateState();
	void writeObject(std::ofstream &file) override;
	RayGenList interaction(Ray &ray, RayGen &rayGen) override;
};

class Mirror : public Object
{
public:
	Mirror();

	void writeObject(std::ofstream &file) override;
	RayGenList interaction(Ray &ray, RayGen &rayGen) override;
};

class Bender : public Object
{
public:
	Bender();

	void writeObject(std::ofstream &file) override;
	RayGenList interaction(Ray &ray, RayGen &rayGen) override;
};

class Splitter : public Object
{
public:
	Splitter();

	void writeObject(std::ofstream &file) override;
	RayGenList interaction(Ray &ray, RayGen &rayGen) override;
};

class Conduit : public Object
{
public:
	Conduit();

	void writeObject(std::ofstream &file) override;
	RayGenList interaction(Ray &ray, RayGen &rayGen) override;
};

class Filter : public Object
{
public:
	Filter(Color col = COL_BLACK_TUPLE);

	void writeObject(std::ofstream &file) override;
	RayGenList interaction(Ray &ray, RayGen &rayGen) override;
};

class Prism : public Object
{
public:
	Prism();

	void writeObject(std::ofstream &file) override;
	RayGenList interaction(Ray &ray, RayGen &rayGen) override;
};

class Doppler : public Object
{
public:
	Doppler();

	void writeObject(std::ofstream &file) override;
	RayGenList interaction(Ray &ray, RayGen &rayGen) override;
};

class Tangler : public Object
{
public:
	Tangler();

	void writeObject(std::ofstream &file) override;
	RayGenList interaction(Ray &ray, RayGen &rayGen) override;
};

class Teleporter : public Object
{
public:
	Teleporter();

	Position findAnotherTeleporter(unsigned short direction);
	void writeObject(std::ofstream &file) override;
	RayGenList interaction(Ray &ray, RayGen &rayGen) override;
};

#endif
