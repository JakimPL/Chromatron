#ifndef COLOR_H
#define COLOR_H

#include <vector>

#include <SFML/Graphics.hpp>
#include "constants.hpp"

class Color
{
	std::vector<bool*> colorTuple = {&red, &green, &blue};
	Colors colorId = COL_BLACK;
public:
	Color(bool r = false, bool g = false, bool b = false);
	Color(Colors colId = COL_BLACK);

	Color operator+(const Color &color);
	Color operator*(const Color &color);
	bool operator==(const Color &color);
	bool operator==(const Colors colID);
	bool operator!=(const Color &color);
	bool operator!=(const Colors colID);

	bool red;
	bool green;
	bool blue;

	sf::Color convertToColor();
	Color shiftColor(bool orientation);
	Color shiftColor(ColorShift colorShift);

	bool isMono();
	void nextColor();
	void getColorId();
	void setColorFromId(Colors colId);
};

static Color colors[COL_COUNT] = {
	Color(COL_BLACK_TUPLE),
	Color(COL_BLUE_TUPLE),
	Color(COL_GREEN_TUPLE),
	Color(COL_CYAN_TUPLE),
	Color(COL_RED_TUPLE),
	Color(COL_MAGENTA_TUPLE),
	Color(COL_YELLOW_TUPLE),
	Color(COL_WHITE_TUPLE)
};

#endif