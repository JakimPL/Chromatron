#ifndef STACK_H
#define STACK_H

#include <SFML/Graphics.hpp>
#include "object.h"

struct Stack {
	unsigned short width = STACK_WIDTH;
	unsigned short height = STACK_HEIGHT;

	Object::Position offset;
	std::map<Object::Position, Object*> objectMap;
	std::map<Object::Position, sf::Sprite> sprites;

	Object::Position getRelativePosition(Object::Position mousePosition);
	bool isFull();
	bool isOnStack(Object::Position position);
	bool isPlaceFree(Object::Position position);
	bool isPlaceTaken(Object::Position position);
};

#endif