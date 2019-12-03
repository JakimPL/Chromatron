#include "constants.h"
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

Color Color::createColor(Colors colorId)
{
	switch (colorId) {
	case Colors::COL_BLACK:
		return Color(COL_BLACK_TUPLE);
	case Colors::COL_RED:
		return Color(COL_RED_TUPLE);
	case Colors::COL_GREEN:
		return Color(COL_GREEN_TUPLE);
	case Colors::COL_BLUE:
		return Color(COL_BLUE_TUPLE);
	case Colors::COL_YELLOW:
		return Color(COL_YELLOW_TUPLE);
	case Colors::COL_MAGENTA:
		return Color(COL_MAGENTA_TUPLE);
	case Colors::COL_CYAN:
		return Color(COL_CYAN_TUPLE);
	case Colors::COL_WHITE:
		return Color(COL_WHITE_TUPLE);
	default:
		return Color(COL_BLACK_TUPLE);
	}
}