#ifndef STACK_H
#define STACK_H

#include <SFML/Graphics.hpp>
#include "object.h"

struct Stack {
	unsigned short width = STACK_WIDTH;
	unsigned short height = STACK_HEIGHT;

	Object::Position offset;
	std::vector<std::vector<Objects>> map;
	void initialize();
};

#endif