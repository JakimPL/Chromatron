#ifndef DRAG_H
#define DRAG_H

#include <SFML/Graphics.hpp>
#include "object.hpp"

struct Drag {
	bool fromStack = false;
	Position position;
	sf::Sprite sprite;
};

#endif