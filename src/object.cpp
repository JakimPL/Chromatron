#include "auxiliary.h"
#include "object.h"

void Dot::updateState()
{
	state = (actualColor == color);
	//sprite.setTexture(*textures[state ? OBJ_DOTF : OBJ_DOT]);
}
