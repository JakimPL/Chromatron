#ifndef AUXILIARY_H
#define AUXILIARY_H

#include <fstream>
#include <SFML/Graphics.hpp>
#include "constants.hpp"
#include "game.hpp"

class Object;

extern void addElement(RayGenList &rayGens, RayGenElement element, unsigned short count = 2);
extern void addNode(Ray &ray, sf::Vertex node, unsigned short count = 2);
extern void addLine(Ray &ray, Color color, Position start, Position end);
extern sf::RectangleShape rectangleCreate(int x, int y, int w, int h, sf::Color color);
extern unsigned short countDigit(unsigned short n);
extern std::string numberToString(unsigned short number);
extern void loadSprites();
extern void readByte(std::ifstream &file, unsigned short &var);
extern void readObject(std::ifstream &file, Level &level, bool stackObject = false);
extern void writeByte(std::ofstream &file, unsigned short var);
extern void writeObject(std::ofstream &file, Object* object);
extern Position floatToPosition(sf::Vector2f vector);
extern Position floatToPosition(float x, float y);
extern Position shortToPosition(short x, short y);
extern sf::Vector2f positionToFloat(Position position);
extern sf::Vector2f positionToFloat(short x, short y);

#endif
