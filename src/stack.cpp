#include "stack.h"
#include "stack.h"

void Stack::initialize()
{
	map.resize(height);
	for (size_t row = 0; row < height; ++row) {
		map[row].resize(width);
	}
}