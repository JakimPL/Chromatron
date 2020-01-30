#include "stack.hpp"
#include "auxiliary.hpp"

Position Stack::getRelativePosition(Position mousePosition)
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

bool Stack::isOnStack(Position position)
{
	Position relativePosition = position - offset;
	short x = relativePosition.getX();
	short y = relativePosition.getY();
	return (x >= 0 && y >= 0 && x < width && y < height);
}

bool Stack::isPlaceFree(Position position)
{
	return objectMap[position] == nullptr;
}

bool Stack::isPlaceTaken(Position position)
{
	return objectMap[position] != nullptr && objectMap[position]->inStack;
}