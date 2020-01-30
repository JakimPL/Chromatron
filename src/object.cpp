#include "object.hpp"
#include "auxiliary.hpp"
#include "constants.hpp"

Object::Object(Color col) : color(col)
{

}

Beamer::Beamer(Color col)
{
	rotatable = false;
	movable = false;
	colorable = true;
	color = col;
}

Dot::Dot(Color col) : actualColor(COL_BLACK_TUPLE)
{
	rotatable = false;
	movable = false;
	colorable = true;
	color = col;
	setSpriteColor();
}

Mirror::Mirror()
{
	rotatable = true;
	movable = true;
	colorable = false;
}

Bender::Bender()
{
	rotatable = true;
	movable = true;
	colorable = false;
}

Splitter::Splitter()
{
	rotatable = true;
	movable = true;
	colorable = false;
}

Conduit::Conduit()
{
	rotatable = false;
	movable = false;
	colorable = false;
}

Filter::Filter(Color col)
{
	rotatable = true;
	movable = true;
	colorable = true;
	color = col;
}

Prism::Prism()
{
	rotatable = true;
	movable = true;
	colorable = false;
}

Doppler::Doppler()
{
	rotatable = true;
	movable = true;
	colorable = false;
}

Tangler::Tangler()
{
	rotatable = true;
	movable = true;
	colorable = false;
}

void Object::rotate(bool clockwise, bool force)
{
	if (rotatable || force) {
		direction = static_cast<DirectionID>((static_cast<unsigned short>(direction) + (clockwise ? 1 : DIR_COUNT - 1)) % DIR_COUNT);
		baseSprite.setRotation(direction * 45);
		sprite.setRotation(direction * 45);
	}
}

void Object::updateSprite()
{
	sprite.setPosition(position);
	baseSprite.setPosition(position);
}

void Object::setSpriteColor()
{
	sprite.setColor(color.convertToColor());
}

void Dot::updateState()
{
	state = (actualColor == color);
	sprite.setTexture(*textures[state ? 1 : 0]);

	direction = DIR_NORTH;
	sprite.setRotation(0);
}