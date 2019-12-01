#include "auxiliary.h"
#include "constants.h"
#include "object.h"

void Dot::updateState()
{
	state = (actualColor == color);
	sprite.setTexture(*textures[state ? 1 : 0]);
}