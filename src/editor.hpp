#ifndef EDITOR_HPP
#define EDITOR_HPP

#include "constants.hpp"

class Game;
class Object;

struct Editor {
private:
	bool active;
	ObjectID currentObject = OBJ_BEAMER;
public:
	EditorMode mode;
	sf::Sprite sprite;
	Game* game;

	bool isActive();
	ObjectID getObject();
	void setObject(ObjectID id);
	void switchMode();
	void turn(bool editorOn);
};

#endif
