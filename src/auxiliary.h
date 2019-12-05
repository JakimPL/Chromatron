#ifndef AUXILIARY_H
#define AUXILIARY_H

#include <fstream>
#include <SFML/Graphics.hpp>
#include "constants.h"
#include "game.h"
#include "gamestate.h"
#include "object.h"

static std::vector<sf::Texture*> textures;

extern sf::RectangleShape rectangleCreate(int x, int y, int w, int h, sf::Color color);
extern sf::Texture loadTexture(const std::string &filename);
extern void readByte(std::ifstream *file, unsigned short &var);
extern void writeByte(std::ofstream *file, unsigned short var);
extern void deleteGameObjects(GameState gameState);
extern void draw(GameState gameState);
extern void drawBoard(GameState gameState);
extern void drawLasers(GameState gameState, bool blackLasers);
extern void drawLasers(GameState gameState);
extern void drawGameObject(GameState gameState);
extern void drawStack(GameState gameState);
extern void drawTile(GameState gameState, Object::Position position, sf::Color outlineColor, sf::Color fillColor);
extern void gameEvents(GameState gameState);
extern void mainLoop(GameState gameState);
extern void keyboardGlobalEvents(GameState gameState);
extern void keyboardEditorEvents(GameState gameState);
extern void mouseEditorEvents(GameState gameState);
extern void mouseGameEvents(GameState gameState);
extern Object::Position floatToPosition(sf::Vector2f vector);
extern Object::Position floatToPosition(float x, float y);
extern sf::Vector2f positionToFloat(Object::Position position);
extern sf::Vector2f positionToFloat(short x, short y);

#endif