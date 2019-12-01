#include "object.h"

void Dot::updateState()
{
	state = (actualColor == color);
}
