#include "structures.h"

Color Color::operator+(const Color& color)
{
	Color newColor(this->red || color.red, this->green || color.green, this->blue || color.blue);
	return newColor;
}

bool Color::operator==(const Color& color)
{
	return (this->red == color.red && this->green == color.green && this->blue == color.blue);
}

sf::Color Color::convertToColor()
{
	return sf::Color(255 * static_cast<sf::Uint8>(red), 255 * static_cast<sf::Uint8>(green), 255 * static_cast<sf::Uint8>(blue));
}