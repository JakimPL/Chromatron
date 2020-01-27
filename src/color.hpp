#ifndef COLOR_H
#define COLOR_H

#include <vector>

#include <SFML/Graphics.hpp>
#include "constants.hpp"

class Color
{
	// Tuple of colors
	std::vector<bool*> colorTuple = {&red, &green, &blue};
	Colors colorId = COL_BLACK;
public:
	Color(bool r = false, bool g = false, bool b = false);
	Color(Colors colId = COL_BLACK);

	// Add two colors (additive), compare two colors
	Color operator+(const Color &color);
	Color operator*(const Color &color);
	bool operator==(const Color &color);

	// Color's components
	bool red;
	bool green;
	bool blue;

	sf::Color convertToColor();
	Color shiftColor(bool orientation);

	void nextColor();
	void getColorId();
	void setColorFromId(Colors colId);
	bool isBlack();
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

typedef std::vector<sf::Vertex> Ray;
typedef std::vector<Ray> Laser;
typedef std::vector<std::pair<unsigned short, bool>> Timeline;

#endif