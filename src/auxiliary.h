#ifndef AUXILIARY_H
#define AUXILIARY_H

#include <fstream>
#include <SFML/Graphics.hpp>

sf::RectangleShape rectangleCreate(int x, int y, int w, int h, sf::Color color);
void readByte(std::ifstream *file, unsigned short &var);

#endif
