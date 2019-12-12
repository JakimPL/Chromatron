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
extern void loadSprites();
extern void readByte(std::ifstream &file, unsigned short &var);
extern void readObject(std::ifstream &file, Game::Level &level);
extern void writeByte(std::ofstream &file, unsigned short var);
extern void writeObject(std::ofstream &file, Object* object);
extern void handleApplicationParameters(GameState gameState, int argc, char* argv[]);
extern void deleteGameObjects(GameState gameState);
extern void draw(GameState gameState);
extern void drawBoard(GameState gameState);
extern void drawLasers(GameState gameState, bool blackLasers);
extern void drawLasers(GameState gameState);
extern void drawGameObjects(GameState gameState);
extern void drawSelector(GameState gameState, Object::Position mousePosition);
extern void drawSelectorSquare(GameState gameState, Object::Position position, sf::Color fillColor, sf::Color outlineColor, bool outlineOnly);
extern void drawStack(GameState gameState);
extern void drawTile(GameState gameState, Object::Position position, bool inStack = false);
extern void gameEvents(GameState gameState);
extern void initializeGame(GameState gameState);
extern void mainLoop(GameState gameState);
extern void nextLevel(GameState gameState);
extern void endGame(GameState gameState);
extern void keyboardGlobalEvents(GameState gameState);
extern void keyboardEditorEvents(GameState gameState);
extern void mouseEditorEvents(GameState gameState);
extern void mouseGameEvents(GameState gameState);
extern void clearLevel(GameState gameState);
extern void resetLevel(GameState gameState);
extern Object::Position floatToPosition(sf::Vector2f vector);
extern Object::Position floatToPosition(float x, float y);
extern Object::Position shortToPosition(short x, short y);
extern sf::Vector2f positionToFloat(Object::Position position);
extern sf::Vector2f positionToFloat(short x, short y);

#endif