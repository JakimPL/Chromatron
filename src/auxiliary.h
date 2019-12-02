#ifndef AUXILIARY_H
#define AUXILIARY_H

#include <fstream>
#include <SFML/Graphics.hpp>

static std::vector<sf::Texture*> textures;

extern sf::RectangleShape rectangleCreate(int x, int y, int w, int h, sf::Color color);
extern sf::Texture loadTexture(const std::string &filename);
extern void readByte(std::ifstream *file, unsigned short &var);
extern void writeByte(std::ofstream *file, unsigned short var);

#endif