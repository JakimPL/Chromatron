#include "constants.h"
#include "structures.h"

Color::Color(bool r, bool g, bool b) : red(r), green(g), blue(b)
{
	getColorId();
}

Color::Color(Colors colId)
{
	setColorFromId(colId);
}

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
	return sf::Color(MAX_COLOR * static_cast<sf::Uint8>(red), MAX_COLOR * static_cast<sf::Uint8>(green), MAX_COLOR * static_cast<sf::Uint8>(blue));
}

void Color::nextColor()
{
	colorId = static_cast<Colors>((colorId + 1) % COL_COUNT);
	setColorFromId(colorId);
}

void Color::getColorId()
{
	unsigned short id = 4 * red + 2 * green + blue;
	colorId = static_cast<Colors>(id);
}

void Color::setColorFromId(Colors colId)
{
	colorId = colId;
	Color color = colors[colorId];

	red = color.red;
	green = color.green;
	blue = color.blue;
}