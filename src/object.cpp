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

	bool flag = true;
	while (flag) {
		xx += moveInDirection_x(dir, 1);
		yy += moveInDirection_y(dir, 1);

		flag = (xx >= 0 && yy >= 0 && xx < game->boardWidth && yy < game->boardHeight);
	}

	xx -= moveInDirection_x(dir, 0.5);
	yy -= moveInDirection_y(dir, 0.5);

	Ray ray(game->OFFSET_X + (xx_start + 0.5) * game->TILE_SIZE, game->OFFSET_Y + (yy_start + 0.5) * game->TILE_SIZE, game->OFFSET_X + (xx + 0.5) * game->TILE_SIZE, game->OFFSET_Y + (yy + 0.5) * game->TILE_SIZE, col);
	laser.push_back(ray);
}

// Move in a direction
float Object::moveInDirection_x(unsigned short dir, float length)
{
	return length * (dir % 4 > 0 ? (dir / 4 > 0 ? -1 : 1) : 0);
}

float Object::moveInDirection_y(unsigned short dir, float length)
{
	return length * ((dir + 2) % 4 > 0 ? ((((dir + 2) % 8) / 4 > 0) ? 1 : -1) : 0);
}