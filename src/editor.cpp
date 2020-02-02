#include "editor.hpp"
#include "game.hpp"

bool Editor::isActive()
{
	return active;
}

void Editor::switchMode()
{
	mode = static_cast<EditorMode>((mode + 1) % static_cast<int>(ED_COUNT));
}

void Editor::turn(bool editorOn)
{
	active = editorOn;
	sprite.setOrigin(ORIGIN);
}

void Editor::setObject(ObjectID id)
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

ObjectID Editor::getObject()
{
	return currentObject;
}