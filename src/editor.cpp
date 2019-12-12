#include "game.h"

bool Game::Editor::isActive()
{
	return active;
}

void Game::Editor::switchMode()
{
	mode = static_cast<EditorMode>((mode + 1) % static_cast<int>(ED_COUNT));
}

void Game::Editor::turn(bool editorOn)
{
	active = editorOn;
	sprite.setOrigin(TILE_SIZE / 2, TILE_SIZE / 2);
}

void Game::Editor::setObject(ObjectID id)
{
	if (active) {
		currentObject = id;
	}

	sprite.setTexture(*game->graphics.textures[id]);
}

ObjectID Game::Editor::getObject()
{
	return currentObject;
}