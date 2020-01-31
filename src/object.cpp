#include "object.hpp"
#include "auxiliary.hpp"
#include "constants.hpp"
#include "log.hpp"
#include <iostream>

Object::Object(Color col) : color(col)
{

}

Object::~Object()
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

void Object::setObject(Game *game, Position position, ObjectID id, DirectionID direction, bool inStack, bool stackObject)
{
	this->id = id;
	this->position.setPosition(position);
	this->direction = direction;
	this->inStack = inStack;

	this->textures.push_back(game->graphics.textures[id]);
	this->sprite.setOrigin(ORIGIN);
	this->sprite.setPosition(inStack ? this->position + game->level.stack.offset : this->position);
	this->sprite.setTexture(*(this->textures)[0]);
	this->sprite.setRotation(direction * HALF_ANGLE);

	if (this->colorable) {
		this->setSpriteColor();
	}

	if (!inStack) {
		game->level.objectMap[this->position] = this;
	} else {
		game->level.stack.objectMap[this->position] = this;
		game->level.stack.objectList[id].push_back(this);
	}

	if (stackObject) {
		game->level.stackObjectList.push_back(this);
	}

	this->setAdditionalSprites(game);
	game->level.objectList[id].push_back(this);
}

void Object::setAdditionalSprites(Game *game)
{

}

void Beamer::setAdditionalSprites(Game *game)
{
	textures.push_back(game->graphics.textures[OBJ_COUNT]);
	baseSprite.setOrigin(ORIGIN);
	baseSprite.setPosition(inStack ? position + game->level.stack.offset : position);
	baseSprite.setTexture(*(textures)[1]);
	baseSprite.setRotation(direction * HALF_ANGLE);
}

void Dot::setAdditionalSprites(Game *game)
{
	textures.push_back(game->graphics.textures[OBJ_COUNT + 1]);
}

void Object::writeGeneralData(std::ofstream &file)
{
	writeByte(file, this->id);
	writeByte(file, this->inStack);
	writeByte(file, this->position.getX());
	writeByte(file, this->position.getY());
}

void Object::writeObject(std::ofstream &file)
{
	writeGeneralData(file);
}

void Beamer::writeObject(std::ofstream &file)
{
	writeGeneralData(file);
	writeByte(file, this->color.red);
	writeByte(file, this->color.green);
	writeByte(file, this->color.blue);
	writeByte(file, this->direction);
}

void Dot::writeObject(std::ofstream &file)
{
	writeGeneralData(file);
	writeByte(file, this->color.red);
	writeByte(file, this->color.green);
	writeByte(file, this->color.blue);
}

void Mirror::writeObject(std::ofstream &file)
{
	writeGeneralData(file);
	writeByte(file, this->direction);
}

void Bender::writeObject(std::ofstream &file)
{
	writeGeneralData(file);
	writeByte(file, this->direction);
}

void Splitter::writeObject(std::ofstream &file)
{
	writeGeneralData(file);
	writeByte(file, this->direction);
}

void Conduit::writeObject(std::ofstream &file)
{
	writeGeneralData(file);
	writeByte(file, this->direction);
}

void Filter::writeObject(std::ofstream &file)
{
	writeGeneralData(file);
	writeByte(file, this->color.red);
	writeByte(file, this->color.green);
	writeByte(file, this->color.blue);
	writeByte(file, this->direction);
}

void Prism::writeObject(std::ofstream &file)
{
	writeGeneralData(file);
	writeByte(file, this->direction);
}

void Doppler::writeObject(std::ofstream &file)
{
	writeGeneralData(file);
	writeByte(file, this->direction);
}

void Tangler::writeObject(std::ofstream &file)
{
	writeGeneralData(file);
	writeByte(file, this->direction);
}

std::vector<RayGenElement> Object::interaction(RayGen &rayGen)
{
	rayGen.stop = rayGen.end = true;
	return {};
}

std::vector<RayGenElement> Beamer::interaction(RayGen &rayGen)
{
	rayGen.stop = rayGen.end = true;
	return {};
}

std::vector<RayGenElement> Dot::interaction(RayGen &rayGen)
{
	this->actualColor = this->actualColor + rayGen.color;
	return {};
}

std::vector<RayGenElement> Mirror::interaction(RayGen &rayGen)
{
	short diff = (DIR_COUNT + this->direction - rayGen.direction) % DIR_COUNT - 4;
	if (ABS(diff) <= 1) {
		rayGen.stop = true;
		rayGen.direction = (DIR_COUNT + rayGen.direction - (diff == 0 ? 4 : 2 * diff)) % DIR_COUNT;
	} else {
		rayGen.stop = rayGen.end = true;
	}

	return {};
}

std::vector<RayGenElement> Bender::interaction(RayGen &rayGen)
{
	short diff = (DIR_COUNT + this->direction - rayGen.direction + 7) % DIR_COUNT - 4;
	if (-2 <= diff && diff < 2) {
		rayGen.stop = true;
		rayGen.direction = (DIR_COUNT + rayGen.direction + (2 * diff + 5)) % DIR_COUNT;
	} else {
		rayGen.stop = rayGen.end = true;
	}

	return {};
}

std::vector<RayGenElement> Splitter::interaction(RayGen &rayGen)
{
	std::vector<RayGenElement> rayGens;
	short diff = (DIR_COUNT + this->direction - rayGen.direction) % (DIR_COUNT / 2) - 2;
	if (diff == 0) {
		rayGen.stop = rayGen.end = true;
	} else if (ABS(diff) == 1) {
		unsigned short newDirection = (DIR_COUNT + rayGen.direction + 2 * diff) % DIR_COUNT;
		RayGen newRayGen = rayGen;
		newRayGen.direction = newDirection;
		rayGens.push_back({newRayGen, RT_NORMAL});
	}

	return rayGens;
}

std::vector<RayGenElement> Conduit::interaction(RayGen &rayGen)
{
	short diff = (DIR_COUNT + this->direction - rayGen.direction) % (DIR_COUNT / 2) - 2;
	if (diff != 0) {
		rayGen.stop = rayGen.end = true;
	}

	return {};
}

std::vector<RayGenElement> Filter::interaction(RayGen &rayGen)
{
	short diff = (DIR_COUNT + this->direction - rayGen.direction + 2) % (DIR_COUNT / 2) - 2;
	if (diff == 0) {
		Color newColor = (this->color * rayGen.color);
		if (newColor != COL_BLACK) {
			if (rayGen.color != newColor) {
				rayGen.color = newColor;
				rayGen.stop = true;
			}
		} else {
			rayGen.stop = rayGen.end = true;
		}
	} else {
		rayGen.stop = rayGen.end = true;
	}

	return {};
}

std::vector<RayGenElement> Prism::interaction(RayGen &rayGen)
{
	std::vector<RayGenElement> rayGens;
	short diff = (DIR_COUNT + this->direction - rayGen.direction) % DIR_COUNT;
	if (rayGen.color.isMono()) {
		if (rayGen.color.red) {
			if ((diff / 2) % 2 != 0) {
			} else {
				rayGen.end = true;
			}
		}
		if (rayGen.color.green) {
			if (diff > 1 && ((diff + 1) % 3 != 1)) {
				rayGen.direction = (DIR_COUNT + rayGen.direction - ((diff + 1) / 3) * 2 + 3) % DIR_COUNT;
			} else {
				rayGen.end = true;
			}
		}
		if (rayGen.color.blue) {
			if (diff > 1 && ((diff + 1) % 3 != 1)) {
				rayGen.direction = (DIR_COUNT + rayGen.direction - ((diff + 1) % 3) * 2 + 2) % DIR_COUNT;
			} else {
				rayGen.end = true;
			}
		}
		rayGen.stop = true;
	} else {
		if (rayGen.color.red) {
			if ((diff / 2) % 2 != 0) {
				RayGen newRayGen = rayGen;
				newRayGen.color = COL_RED;
				rayGens.push_back({newRayGen, RT_NORMAL});
			}
		}
		if (rayGen.color.green) {
			if (diff > 1 && ((diff + 1) % 3 != 1)) {
				RayGen newRayGen = rayGen;
				newRayGen.direction = (DIR_COUNT + rayGen.direction - ((diff + 1) / 3) * 2 + 3) % DIR_COUNT;
				newRayGen.color = COL_GREEN;
				rayGens.push_back({newRayGen, RT_NORMAL});
			}
		}
		if (rayGen.color.blue) {
			if (diff > 1 && ((diff + 1) % 3 != 1)) {
				RayGen newRayGen = rayGen;
				newRayGen.direction = (DIR_COUNT + rayGen.direction - ((diff + 1) % 3) * 2 + 2) % DIR_COUNT;
				newRayGen.color = COL_BLUE;
				rayGens.push_back({newRayGen, RT_NORMAL});
			}
		}
		rayGen.stop = rayGen.end = true;
	}

	return rayGens;
}

std::vector<RayGenElement> Doppler::interaction(RayGen &rayGen)
{
	short diff = (DIR_COUNT + this->direction - rayGen.direction) % DIR_COUNT - 4;
	if ((diff + 2) % 4 == 0) {
		rayGen.colorShift = static_cast<ColorShift>(1 + ((diff + 2) / 4));
		Color newColor = rayGen.color.shiftColor(rayGen.colorShift);
		if (newColor != COL_WHITE) {
			rayGen.color = newColor;
			rayGen.stop = true;
		}
	} else {
		rayGen.stop = rayGen.end = true;
	}

	return {};
}

std::vector<RayGenElement> Tangler::interaction(RayGen &rayGen)
{
	std::vector<RayGenElement> rayGens;
	short diff = (DIR_COUNT + this->direction - rayGen.direction) % DIR_COUNT - 4;
	if (diff == 0) {
		if (rayGen.color.red) {
			rayGens.push_back({{rayGen.direction, rayGen.position, COL_RED}, RT_TANGLED});
		}
		if (rayGen.color.green) {
			rayGens.push_back({{rayGen.direction, rayGen.position, COL_GREEN}, RT_TANGLED});
		}
		if (rayGen.color.blue) {
			rayGens.push_back({{rayGen.direction, rayGen.position, COL_BLUE}, RT_TANGLED});
		}
	}
	rayGen.stop = rayGen.end = true;

	return rayGens;
}

void Object::rotate(bool clockwise, bool force)
{
	if (rotatable || force) {
		direction = static_cast<DirectionID>((static_cast<unsigned short>(direction) + (clockwise ? 1 : DIR_COUNT - 1)) % DIR_COUNT);
		baseSprite.setRotation(direction * HALF_ANGLE);
		sprite.setRotation(direction * HALF_ANGLE);
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