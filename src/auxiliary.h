#ifndef AUXILIARY_H
#define AUXILIARY_H

#include <fstream>
#include <SFML/Graphics.hpp>

extern sf::RectangleShape rectangleCreate(int x, int y, int w, int h, sf::Color color);
extern sf::Texture* loadTexture(const std::string &filename);
extern void readByte(std::ifstream *file, unsigned short &var);

#endif
