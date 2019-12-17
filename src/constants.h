#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <SFML/Graphics.hpp>

#define SCREEN_WIDTH 600
#define SCREEN_HEIGHT 420
#define OFFSET_X 1
#define OFFSET_Y 1
#define TILE_SIZE 24
#define OUTLINE_SIZE 1

#define STACK_WIDTH 6
#define STACK_HEIGHT 4
#define STACK_OFFSET_X 1
#define STACK_OFFSET_Y 0

#define MAX_COLOR 255
#define ORIGIN TILE_SIZE / 2, TILE_SIZE / 2

#define LogInfo(message) Log::log(Log::Type::INFO, message)
#define LogError(message) Log::log(Log::Type::ERROR, message)
#define LogMessage(message) Log::log(Log::Type::MESSAGE, message)
#define LogWarning(message) Log::log(Log::Type::WARNING, message)
#define LogNone(message) Log::log(Log::Type::NONE, message)

#define COL_BLACK_TUPLE   {false, false, false}
#define COL_RED_TUPLE     {true,  false, false}
#define COL_GREEN_TUPLE   {false, true,  false}
#define COL_BLUE_TUPLE    {false, false, true}
#define COL_YELLOW_TUPLE  {true,  true,  false}
#define COL_MAGENTA_TUPLE {true,  false, true}
#define COL_CYAN_TUPLE    {false, true,  true}
#define COL_WHITE_TUPLE   {true,  true,  true}

#define dRED        "\x1b[0;31;40m"
#define dGREEN      "\x1b[0;32;40m"
#define dYELLOW     "\x1b[0;33;40m"
#define dBLUE       "\x1b[0;34;40m"
#define dMAGENTA    "\x1b[0;35;40m"
#define dCYAN       "\x1b[0;36;40m"
#define dWHITE      "\x1b[0;37;40m"

#define lRED		"\033[1;31m"
#define lGREEN		"\033[1;32m"
#define lYELLOW		"\033[1;33m"
#define lBLUE		"\033[1;34m"
#define lMAGENTA	"\033[1;35m"
#define lCYAN		"\033[1;36m"
#define lWHITE		"\033[1;37m"

#define RESET 	    "\x1b[0;7;0m"

enum Colors : unsigned short {
	COL_BLACK,
	COL_BLUE,
	COL_GREEN,
	COL_CYAN,
	COL_RED,
	COL_MAGENTA,
	COL_YELLOW,
	COL_WHITE,
	COL_COUNT
};

enum DirectionID : unsigned short {
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

enum ObjectID : unsigned short {
	OBJ_EMPTY,
	OBJ_BEAMER,
	OBJ_DOT,
	OBJ_MIRROR,
	OBJ_BENDER,
	OBJ_SPLITTER,
	OBJ_CONDUIT,
	OBJ_FILTER,
	OBJ_COUNT
};

enum EditorMode : unsigned short {
	ED_EDIT_OBJECTS,
	ED_ADD_OR_REMOVE_OBJECTS,
	ED_ADD_OR_REMOVE_OBSTACLES,
	ED_COUNT
};

enum Tile : unsigned short {
	TIL_EMPTY,
	TIL_OBSTACLE,
	TIL_COUNT
};

enum LevelState : unsigned short {
	LS_LOCKED,
	LS_AVAILABLE,
	LS_PASSED
};

static std::string PATH_DATA       = "./data/";
static std::string PATH_LEV_PREFIX = "LEV/";
static std::string PATH_LEV_SUFFIX = ".lev";
static std::string PATH_IMG_PREFIX = "IMG/";
static std::string PATH_IMG_SUFFIX = ".png";
static std::string PATH_LS_SUFFIX  = "/levels";
static std::string FIRST_LEVEL_ID  = "001";
static std::string PATH_SAV_SUFFIX = ".sav";
static std::string PATH_SET_SUFFIX = ".set";
static std::string PATH_LOG_FILE   = "Chromatron.log";
static std::vector<std::string> IMG_OBJECT_NAMES = {"empty", "beamer", "dotB", "mirror", "bender", "splitter", "conduit", "filter", "dotF"};
static std::vector<std::string> IMG_TILE_NAMES = {"tileEmpty", "tileObstacle"};

static sf::Color   black(0,   0,   0);
static sf::Color   dgray(96,  96,  96);
static sf::Color    gray(128, 128, 128);
static sf::Color   lgray(192, 192, 192);
static sf::Color   white(255, 255, 255);
static sf::Color    lred(255, 128, 128);
static sf::Color     red(255, 0,   0);
static sf::Color    dred(128, 0,   0);
static sf::Color   green(0,   255, 0);
static sf::Color  lgreen(128, 255, 128);
static sf::Color  dgreen(0,   128, 0);
static sf::Color   lblue(128, 128, 255);
static sf::Color    blue(0,   0,   255);
static sf::Color   dblue(0,   0,   128);
static sf::Color  yellow(255, 255, 0);
static sf::Color dyellow(128, 128, 0);

#endif