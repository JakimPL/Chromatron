#include "auxiliary.h"

// Create a rectangle
sf::RectangleShape rectangleCreate(int x, int y, int w, int h, sf::Color color)
{
	sf::RectangleShape rectangle(sf::Vector2f(w, h));
	rectangle.setPosition(x, y);
	rectangle.setFillColor(color);
	return rectangle;
}

// Read a byte from a file
void readByte(std::ifstream* file, unsigned short &var)
{
	char buffer;
	file->read(&buffer, 1);
	var = static_cast<unsigned short>(buffer);
}