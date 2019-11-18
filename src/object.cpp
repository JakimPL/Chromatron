#include "object.h"

void Object::initialize(Game* gm, unsigned short xx, unsigned short yy)
{
	x = xx;
	y = yy;
	game = gm;
	sprite.setPosition(game->OFFSET_X + game->TILE_SIZE * x, game->OFFSET_Y + game->TILE_SIZE * y);
	sprite.setTexture(*(gm->textures[id]));
}