#include "auxiliary.h"
#include "constants.h"
#include "object.h"

void Object::rotate(bool clockwise)
{
	if (rotatable) {
		direction = (direction + (clockwise ? 1 : DIRS - 1)) % DIRS;
		sprite.setRotation(direction * 45);
	}
}

void Dot::updateState()
{
	state = (actualColor == color);
	sprite.setTexture(*textures[state ? 1 : 0]);
}