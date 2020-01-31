#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <SFML/Graphics.hpp>

#define SCREEN_WIDTH 600
#define SCREEN_HEIGHT 420
#define OFFSET_X 1
#define OFFSET_Y 1
#define TILE_SIZE 24
#define OUTLINE_SIZE 1
#define HALF_ANGLE 45

#define STACK_WIDTH 6
#define STACK_HEIGHT 4
#define STACK_OFFSET_X 1
#define STACK_OFFSET_Y 0

#define MAX_COLOR 255
#define ORIGIN TILE_SIZE / 2, TILE_SIZE / 2
#define ABS(a) std::abs(a)
#define DIR(a) static_cast<unsigned short>((DIR_COUNT + ((a) % DIR_COUNT)) % DIR_COUNT)
#define CLS_REVERSE(a) static_cast<ColorShift>((3 - ((3 + (a % 3)) % 3)) % 3)

#define LogInfo(message) Log::log(Log::Type::INFO, message)
#define LogError(message) Log::log(Log::Type::ERROR, message)
#define LogMessage(message) Log::log(Log::Type::MESSAGE, message)
#define LogWarning(message) Log::log(Log::Type::WARNING, message)
#define LogNone(message) Log::log(Log::Type::NONE, message)

#define BASIC_COLORS 3
#define COL_BLACK_TUPLE   {false, false, false}
#define COL_RED_TUPLE     {true,  false, false}
#define COL_GREEN_TUPLE   {false, true,  false}
#define COL_BLUE_TUPLE    {false, false, true}
#define COL_YELLOW_TUPLE  {true,  true,  false}
#define COL_MAGENTA_TUPLE {true,  false, true}
#define COL_CYAN_TUPLE    {false, true,  true}
#define COL_WHITE_TUPLE   {true,  true,  true}

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

enum ColorShift : unsigned short {
	CLS_NONE,
	CLS_RIGHT,
	CLS_LEFT
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
	OBJ_PRISM,
	OBJ_DOPPLER,
	OBJ_TANGLER,
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

enum RayType : unsigned short {
	RT_NORMAL,
	RT_TANGLED
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
static std::vector<std::string> IMG_OBJECT_NAMES = {"empty", "beamerDot", "dotEmpty", "mirror", "bender", "splitter", "conduit", "filter", "prism", "doppler", "tangler", "beamer", "dotFull"};
static std::vector<std::string> IMG_TILE_NAMES   = {"tileEmpty", "tileObstacle"};

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