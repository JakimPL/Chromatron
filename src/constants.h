#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <SFML/Graphics.hpp>

#define OBJ_DOTF 5

#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 480
#define OFFSET_X 32
#define OFFSET_Y 32
#define TILE_SIZE 24
#define OUTLINE_SIZE 1

#define NULLPOSITION {}

#define COL_BLACK {false, false, false}

static std::string PATH_DATA = "./data/";
static std::string PATH_LEV_PREFIX = "LEV/level";
static std::string PATH_LEV_SUFFIX = ".lev";
static std::string PATH_IMG_PREFIX = "IMG/";
static std::string PATH_IMG_SUFFIX = ".png";
static std::vector<std::string> IMG_NAMES = {"empty", "beamer", "dotB", "mirror", "bender", "dotF"};

typedef sf::Keyboard Key;
typedef sf::Event Ev;

static sf::Color black(0, 0, 0);
static sf::Color gray(128, 128, 128);
static sf::Color lgray(192, 192, 192);
static sf::Color dgray(96, 96, 96);
static sf::Color white(255, 255, 255);
static sf::Color yellow(255, 255, 0);
static sf::Color red(255, 0, 0);
static sf::Color green(0, 255, 0);
static sf::Color blue(0, 0, 255);
static sf::Color lred(255, 128, 128);
static sf::Color lgreen(128, 255, 128);
static sf::Color lblue(128, 128, 255);

enum Objects {
	OBJ_EMPTY,
	OBJ_BEAMER,
	OBJ_DOT,
	OBJ_MIRROR,
	OBJ_BENDER,
	OBJ_COUNT,
};

enum Directions {
	DIR_NORTH,
	DIR_NORTHEAST,
	DIR_EAST,
	DIR_SOUTHEAST,
	DIR_SOUTH,
	DIR_SOUTHWEST,
	DIR_WEST,
	DIR_NORTHWEST,
	DIR_COUNT
};

#endif