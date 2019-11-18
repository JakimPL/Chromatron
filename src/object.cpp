#include "object.h"

void Object::initialize(Game* gm, unsigned short xx, unsigned short yy)
{
	x = xx;
	y = yy;
	game = gm;
	sprite.setPosition(game->OFFSET_X + game->TILE_SIZE * x, game->OFFSET_Y + game->TILE_SIZE * y);
	sprite.setTexture(*(gm->textures[id]));
}

void Beamer::calculateLaser()
{
	float xx_start, xx, yy_start, yy;
	xx = xx_start = static_cast<float>(x);
	yy = yy_start = static_cast<float>(y);
	Color col = color;
	unsigned short dir = direction;
	while (xx >= 0 && yy >= 0 && xx < game->boardWidth && yy < game->boardHeight) {
		xx += (dir % 4 > 0 ? (dir / 4 > 0 ? -1 : 1) : 0);
		yy += ((dir + 2) % 4 > 0 ? ((((dir + 2) % 8) / 4 > 0) ? 1 : -1) : 0);
	}
	Ray ray(game->OFFSET_X + (xx_start + 0.5) * game->TILE_SIZE, game->OFFSET_Y + (yy_start + 0.5) * game->TILE_SIZE, game->OFFSET_X + (xx + 0.5) * game->TILE_SIZE, game->OFFSET_Y + (yy + 0.5) * game->TILE_SIZE, col);
	laser.push_back(ray);
}