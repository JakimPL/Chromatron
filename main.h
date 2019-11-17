#ifndef MAIN_H
#define MAIN_H

#include <SFML/Graphics.hpp>
#include "src/auxiliary.h"
#include "src/level.h"
#include "src/object.h"
#include "src/structures.h"

#define OBJ_BEAMER 1
#define OBJ_DOT 2
#define OBJ_MIRROR 3
#define OBJ_BENDER 4

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

constexpr int SCREEN_WIDTH = 1024;
constexpr int SCREEN_HEIGHT = 768;
constexpr int OFFSET_X = 64;
constexpr int OFFSET_Y = 64;
constexpr int TILE_SIZE = 40;
constexpr int OUTLINE_SIZE = 1;

const std::string PATH_DATA_PREFIX = "./data/LEV/level";
const std::string PATH_DATA_SUFFIX = ".lev";

#endif