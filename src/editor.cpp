#include "game.hpp"

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
	sprite.setOrigin(ORIGIN);
}

void Game::Editor::setObject(ObjectID id)
{
	if (active) {
		currentObject = id;
	}

	if (id == OBJ_BEAMER) {
		sprite.setTexture(*game->graphics.textures[OBJ_COUNT]);
	} else {
		sprite.setTexture(*game->graphics.textures[id]);
	}
}

ObjectID Game::Editor::getObject()
{
	return currentObject;
}