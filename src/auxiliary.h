#ifndef AUXILIARY_H
#define AUXILIARY_H

#include <fstream>
#include <SFML/Graphics.hpp>
#include "constants.h"
#include "game.h"
#include "gamestate.h"
#include "object.h"

extern sf::RectangleShape rectangleCreate(int x, int y, int w, int h, sf::Color color);
extern unsigned short countDigit(unsigned short n);
extern std::string numberToString(unsigned short number);
extern void loadSprites();
extern void readByte(std::ifstream &file, unsigned short &var);
extern void readObject(std::ifstream &file, Game::Level &level);
extern void writeByte(std::ofstream &file, unsigned short var);
extern void writeObject(std::ofstream &file, Object* object);
extern Object::Position floatToPosition(sf::Vector2f vector);
extern Object::Position floatToPosition(float x, float y);
extern Object::Position shortToPosition(short x, short y);
extern sf::Vector2f positionToFloat(Object::Position position);
extern sf::Vector2f positionToFloat(short x, short y);

#endif