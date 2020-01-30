#ifndef STACK_H
#define STACK_H

#include <SFML/Graphics.hpp>
#include "object.hpp"

struct Stack {
	unsigned short width = STACK_WIDTH;
	unsigned short height = STACK_HEIGHT;

	Position offset;
	std::vector<Object*> objectList[OBJ_COUNT];
	std::map<Position, Object*> objectMap;
	std::map<Position, sf::Sprite> sprites;

	Position getRelativePosition(Position mousePosition);
	bool isFull();
	bool isOnStack(Position position);
	bool isPlaceFree(Position position);
	bool isPlaceTaken(Position position);
};

#endif