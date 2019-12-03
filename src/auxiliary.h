#ifndef AUXILIARY_H
#define AUXILIARY_H

#include <fstream>
#include <SFML/Graphics.hpp>
#include "constants.h"
#include "game.h"
#include "object.h"

static std::vector<sf::Texture*> textures;

extern sf::RectangleShape rectangleCreate(int x, int y, int w, int h, sf::Color color);
extern sf::Texture loadTexture(const std::string &filename);
extern void readByte(std::ifstream *file, unsigned short &var);
extern void writeByte(std::ofstream *file, unsigned short var);
extern void deleteGameObjects(Game &game);
extern void draw(Game &game, sf::RenderWindow &window, Drag &drag, Object::Position mousePosition);
extern void drawBoard(Game &game, sf::RenderWindow &window, Object::Position mousePosition);
extern void drawLasers(Game &game, sf::RenderWindow &window, bool blackLasers);
extern void drawLasers(Game &game, sf::RenderWindow &window);
extern void drawGameObject(Game &game, sf::RenderWindow &window, Drag &drag, Object::Position mousePosition);
extern void drawStack(Game &game, sf::RenderWindow &window);
extern void gameEvents(Game &game, Ev &event, Drag &drag, Object::Position position);
extern void keyboardGlobalEvents(Game &game, Ev &event);
extern void keyboardEditorEvents(Game &game, Ev &event);
extern void mouseEditorEvents(Game &game, Ev &event, Object::Position mousePosition);
extern void mouseGameEvents(Game &game, Ev &event, Drag &drag, Object::Position mousePosition);

#endif