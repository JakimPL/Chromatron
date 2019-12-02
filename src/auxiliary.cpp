#include "auxiliary.h"
#include "constants.h"

sf::RectangleShape rectangleCreate(int x, int y, int w, int h, sf::Color color)
{
	sf::RectangleShape rectangle(sf::Vector2f(w, h));
	rectangle.setPosition(x, y);
	rectangle.setFillColor(color);
	return rectangle;
}

void readByte(std::ifstream* file, unsigned short &var)
{
	char buffer;
	file->read(&buffer, 1);
	var = static_cast<unsigned short>(buffer);
}

bool isMouseOn(short x, short y, sf::Vector2f mousePosition)
{
	return (mousePosition.x >= OFFSET_X + TILE_SIZE * x && mousePosition.x < OFFSET_X + TILE_SIZE * (x + 1) && mousePosition.y >= OFFSET_Y + TILE_SIZE * y && mousePosition.y < OFFSET_Y + TILE_SIZE * (y + 1));
}