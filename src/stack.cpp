#include "stack.hpp"
#include "auxiliary.hpp"

Object::Position Stack::getRelativePosition(Object::Position mousePosition)
{
	return mousePosition - offset;
}

bool Stack::isFull()
{
	for (short y = 0; y < height; ++y) {
		for (short x = 0; x < width; ++x) {
			if (objectMap[shortToPosition(x, y)] == nullptr) {
				return false;
			}
		}
	}

	return true;
}

bool Stack::isOnStack(Object::Position position)
{
	Object::Position relativePosition = position - offset;
	short x = relativePosition.getX();
	short y = relativePosition.getY();
	return (x >= 0 && y >= 0 && x < width && y < height);
}

bool Stack::isPlaceFree(Object::Position position)
{
	return objectMap[position] == nullptr;
}

bool Stack::isPlaceTaken(Object::Position position)
{
	return objectMap[position] != nullptr && objectMap[position]->inStack;
}