#ifndef DRAG_H
#define DRAG_H

#include <SFML/Graphics.hpp>
#include "object.h"

struct Drag {
	Object::Position position;
	sf::Sprite sprite;
};

#endif