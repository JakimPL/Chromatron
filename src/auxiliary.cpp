#include "auxiliary.h"

#include <iostream>

sf::RectangleShape rectangleCreate(int x, int y, int w, int h, sf::Color color)
{
	sf::RectangleShape rectangle(sf::Vector2f(w, h));
	rectangle.setPosition(x, y);
	rectangle.setFillColor(color);
	return rectangle;
}

void readByte(std::ifstream &file, unsigned short &var)
{
	char buffer;
	file.read(&buffer, 1);
	var = static_cast<unsigned short>(buffer);
}

void readObject(std::ifstream &file, Game::Level &level)
{
	unsigned short id;
	readByte(file, id);

	unsigned short inStack;
	readByte(file, inStack);

	unsigned short x, y;
	readByte(file, x);
	readByte(file, y);

	if (id == OBJ_BEAMER) {
		unsigned short red, green, blue;
		unsigned short direction;

		readByte(file, red);
		readByte(file, green);
		readByte(file, blue);
		readByte(file, direction);

		Color color(red > 0, green > 0, blue > 0);

		Beamer* beamer = new Beamer(color);
		level.setObject(beamer, x, y, static_cast<ObjectID>(id), static_cast<DirectionID>(direction), inStack > 0);
	} else if (id == OBJ_DOT) {
		unsigned short red, green, blue;

		readByte(file, red);
		readByte(file, green);
		readByte(file, blue);

		Color color(red > 0, green > 0, blue > 0);

		Dot* dot = new Dot(color);
		level.setObject(dot, x, y, static_cast<ObjectID>(id), DIR_NORTH, inStack > 0);
	} else if (id == OBJ_MIRROR) {
		unsigned short direction;

		readByte(file, direction);

		Mirror* mirror = new Mirror();
		level.setObject(mirror, x, y, static_cast<ObjectID>(id), static_cast<DirectionID>(direction), inStack > 0);
	} else if (id == OBJ_BENDER) {
		unsigned short direction;

		readByte(file, direction);

		Bender* bender = new Bender();
		level.setObject(bender, x, y, static_cast<ObjectID>(id), static_cast<DirectionID>(direction), inStack > 0);
	}
}

void writeObject(std::ofstream &file, Object* object)
{
	if (object != nullptr) {
		writeByte(file, object->id);
		writeByte(file, object->inStack);
		writeByte(file, object->position.getX());
		writeByte(file, object->position.getY());

		if (object->id == OBJ_BEAMER) {
			Beamer* beamer = (Beamer*) object;

			writeByte(file, beamer->color.red);
			writeByte(file, beamer->color.green);
			writeByte(file, beamer->color.blue);
			writeByte(file, beamer->direction);
		} else if (object->id == OBJ_DOT) {
			Dot* dot = (Dot*) object;

			writeByte(file, dot->color.red);
			writeByte(file, dot->color.green);
			writeByte(file, dot->color.blue);
		} else if (object->id == OBJ_MIRROR) {
			Mirror* mirror = (Mirror*) object;

			writeByte(file, mirror->direction);
		} else if (object->id == OBJ_BENDER) {
			Bender* bender = (Bender*) object;

			writeByte(file, bender->direction);
		}
	} else {
		writeByte(file, OBJ_EMPTY);
	}
}

void writeByte(std::ofstream &file, unsigned short var)
{
	char buffer = static_cast<char>(var);
	file.write(&buffer, 1);
}

void handleApplicationParameters(GameState gameState, int argc, char* argv[])
{
	bool editorOn = false;
	for (int arg = 0; arg < argc; ++arg) {
		editorOn = std::string(argv[arg]) == "--editor";
	}

	gameState.game.editor.turn(editorOn);
}

void initializeGame(GameState gameState)
{
	gameState.window.setView(sf::View(sf::FloatRect(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT)));
	gameState.game.loadSet("Levelset");
}

void mainLoop(GameState gameState)
{
	while (gameState.window.isOpen()) {
		gameState.update();
		while (gameState.window.pollEvent(gameState.event)) {
			if (gameState.event.type == sf::Event::Closed or sf::Keyboard::isKeyPressed(sf::Keyboard::Escape)) {
				gameState.window.close();
				break;
			}

			gameEvents(gameState);
			draw(gameState);
		}
	}
}

void endGame(GameState gameState)
{
	gameState.game.saveSet(gameState.game.levelSet.name);
	deleteGameObjects(gameState);
}

void deleteGameObjects(GameState gameState)
{
	for (size_t type = 0; type < OBJ_COUNT; ++type) {
		for (size_t index = 0; index < gameState.game.level.objectList[type].size(); ++index) {
			delete gameState.game.level.objectList[type][index];
		}
	}
}

void draw(GameState gameState)
{
	gameState.window.clear();

	drawBoard(gameState);
	drawStack(gameState);
	drawLasers(gameState);
	drawGameObjects(gameState);

	gameState.window.display();
}

void drawBoard(GameState gameState)
{
	gameState.window.draw(rectangleCreate(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, lgray));

	for (short y = 0; y < gameState.game.level.height; ++y) {
		for (short x = 0; x < gameState.game.level.width; ++x) {
			Object::Position currentPosition = shortToPosition(x, y);
			drawTile(gameState, currentPosition);
		}
	}

	if (!gameState.game.level.isOutsideBoard(gameState.mousePosition)) {
		drawSelector(gameState, gameState.mousePosition);
	}
}

void drawLasers(GameState gameState)
{
	drawLasers(gameState, true);
	drawLasers(gameState, false);
}

void drawLasers(GameState gameState, bool blackLasers)
{
	for (size_t beamerIndex = 0; beamerIndex < gameState.game.level.objectList[OBJ_BEAMER].size(); ++beamerIndex) {
		Beamer* beamer = (Beamer*) gameState.game.level.objectList[OBJ_BEAMER][beamerIndex];
		size_t rays = beamer->laser.size();

		for (size_t rayIndex = 0; rayIndex < rays; ++rayIndex) {
			size_t size = beamer->laser[rayIndex].size();
			sf::Vertex vertices[size];
			std::copy(beamer->laser[rayIndex].begin(), beamer->laser[rayIndex].end(), vertices);
			if (blackLasers) {
				for (size_t node = 0; node < size; ++node) {
					vertices[node].color = black;
				}
			}

			gameState.window.draw(vertices, size, sf::LinesStrip, (blackLasers ? sf::BlendNone : sf::BlendAdd));
		}
	}
}

void drawGameObjects(GameState gameState)
{
	bool shiftPressed = (sf::Keyboard::isKeyPressed(sf::Keyboard::LShift));
	for (size_t type = 0; type < OBJ_COUNT; ++type) {
		for (size_t index = 0; index < gameState.game.level.objectList[type].size(); ++index) {
			if (gameState.drag.fromStack != gameState.game.level.objectList[type][index]->inStack || gameState.drag.position != gameState.game.level.objectList[type][index]->position) {
				gameState.window.draw(gameState.game.level.objectList[type][index]->sprite);
			}
		}
	}

	if (gameState.game.editor.isActive() && !shiftPressed) {
		if (gameState.game.editor.mode == ED_ADD_OR_REMOVE_OBJECTS) {
			gameState.game.editor.sprite.setPosition(gameState.mousePosition);
			gameState.window.draw(gameState.game.editor.sprite);
		}
	} else {
		if (!gameState.drag.position.isNull()) {
			gameState.drag.sprite.setPosition(gameState.mousePosition);
			gameState.window.draw(gameState.drag.sprite);
		}
	}
}

void drawSelector(GameState gameState, Object::Position mousePosition)
{
	sf::Color fillColor, outlineColor;
	bool outlineOnly = false;
	if (gameState.game.editor.isActive()) {
		switch (gameState.game.editor.mode) {
		case ED_EDIT_OBJECTS: {
			fillColor = dyellow;
			outlineColor = lgray;
			break;
		}
		case ED_ADD_OR_REMOVE_OBJECTS: {
			fillColor = dred;
			outlineColor = gray;
			break;
		}
		default:
			outlineColor = lgray;
			outlineOnly = true;
			break;
		}
	} else {
		fillColor = dyellow;
		outlineColor = lgray;
	}

	drawSelectorSquare(gameState, mousePosition, fillColor, outlineColor, outlineOnly);
}

void drawStack(GameState gameState)
{
	for (short y = 0; y < gameState.game.level.stack.height; ++y) {
		for (short x = 0; x < gameState.game.level.stack.width; ++x) {
			Object::Position currentPosition = shortToPosition(x, y);
			drawTile(gameState, currentPosition, true);
		}
	}

	if (gameState.game.level.stack.isOnStack(gameState.mousePosition)) {
		drawSelector(gameState, gameState.mousePosition);
	}
}

void drawTile(GameState gameState, Object::Position position, bool inStack)
{
	sf::Sprite sprite = (inStack ? gameState.game.level.stack.sprites[position] : gameState.game.level.tileSprites[position]);
	gameState.window.draw(sprite);
}

void drawSelectorSquare(GameState gameState, Object::Position position, sf::Color fillColor, sf::Color outlineColor, bool outlineOnly)
{
	sf::Vector2f realPosition(position);
	realPosition.x -= TILE_SIZE / 2;
	realPosition.y -= TILE_SIZE / 2;
	if (!outlineOnly) {
		gameState.window.draw(rectangleCreate(realPosition.x, realPosition.y, TILE_SIZE, TILE_SIZE, outlineColor));
		gameState.window.draw(rectangleCreate(realPosition.x + OUTLINE_SIZE, realPosition.y + OUTLINE_SIZE, TILE_SIZE - 2 * OUTLINE_SIZE, TILE_SIZE - 2 * OUTLINE_SIZE, fillColor));
	} else {
		sf::Vertex outline[] = {
			sf::Vertex(sf::Vector2f(realPosition) + sf::Vector2f(1, 1)),
			sf::Vertex(sf::Vector2f(realPosition) + sf::Vector2f(TILE_SIZE, 1)),
			sf::Vertex(sf::Vector2f(realPosition) + sf::Vector2f(TILE_SIZE, TILE_SIZE)),
			sf::Vertex(sf::Vector2f(realPosition) + sf::Vector2f(1, TILE_SIZE)),
			sf::Vertex(sf::Vector2f(realPosition))
		};

		for (size_t node = 0; node < 5; ++node) {
			outline[node].color = outlineColor;
		}

		gameState.window.draw(outline, 5, sf::LinesStrip);
	}
}

void gameEvents(GameState gameState)
{
	keyboardGlobalEvents(gameState);

	bool shiftPressed = (sf::Keyboard::isKeyPressed(sf::Keyboard::LShift));
	if (gameState.game.editor.isActive() && !shiftPressed) {
		keyboardEditorEvents(gameState);
		mouseEditorEvents(gameState);
	} else {
		mouseGameEvents(gameState);
	}

	if (gameState.game.level.event) {
		gameState.game.calculateLasers();



		if (gameState.game.level.checkWin()) {
			gameState.game.levelSet.levelStates[gameState.game.levelSet.currentLevel] = LS_PASSED;
			gameState.game.saveSet();
		}

		gameState.game.level.event = false;
	}
}

void keyboardGlobalEvents(GameState gameState)
{
	if (gameState.event.type == sf::Event::KeyPressed) {
		switch (gameState.event.key.code) {
		case sf::Keyboard::C: {
			clearLevel(gameState);
			break;
		}
		case sf::Keyboard::E: {
			gameState.game.editor.turn(!gameState.game.editor.isActive());
			break;
		}
		case sf::Keyboard::R: {
			resetLevel(gameState);
			break;
		}
		case sf::Keyboard::S: {
			if (gameState.game.editor.isActive()) {
				gameState.game.saveLevel("999");
			}
			break;
		}
		default:
			break;
		}
	}
}

void keyboardEditorEvents(GameState gameState)
{
	if (gameState.event.type == sf::Event::KeyPressed) {
		switch (gameState.event.key.code) {
		case sf::Keyboard::Tab: {
			gameState.game.editor.switchMode();
			break;
		}
		case sf::Keyboard::Num1: {
			gameState.game.editor.setObject(OBJ_BEAMER);
			break;
		}
		case sf::Keyboard::Num2: {
			gameState.game.editor.setObject(OBJ_DOT);
			break;
		}
		case sf::Keyboard::Num3: {
			gameState.game.editor.setObject(OBJ_MIRROR);
			break;
		}
		case sf::Keyboard::Num4: {
			gameState.game.editor.setObject(OBJ_BENDER);
			break;
		}
		default:
			break;
		}
	}
}

void mouseEditorEvents(GameState gameState)
{
	if (gameState.game.editor.mode == ED_EDIT_OBJECTS) {
		if (gameState.event.type == sf::Event::MouseButtonPressed) {
			if (gameState.event.mouseButton.button == sf::Mouse::Left) {
				gameState.game.level.event = gameState.game.level.rotateObject(gameState.mousePosition);
			} else if (gameState.event.mouseButton.button == sf::Mouse::Right) {
				gameState.game.level.event = gameState.game.level.changeObjectColor(gameState.mousePosition);
			}
		}
	} else if (gameState.game.editor.mode == ED_ADD_OR_REMOVE_OBJECTS) {
		if (gameState.event.type == sf::Event::MouseButtonPressed) {
			if (gameState.event.mouseButton.button == sf::Mouse::Left) {
				gameState.game.level.event = gameState.game.level.addObject(gameState.mousePosition, gameState.game.editor.getObject());
			} else if (gameState.event.mouseButton.button == sf::Mouse::Right) {
				gameState.game.level.event = gameState.game.level.removeObject(gameState.mousePosition);
			}
		}
	} else if (gameState.game.editor.mode == ED_ADD_OR_REMOVE_OBSTACLES) {
		if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
			gameState.game.level.event = gameState.game.level.setObstacle(gameState.mousePosition, true);
		} else if (sf::Mouse::isButtonPressed(sf::Mouse::Right)) {
			gameState.game.level.event = gameState.game.level.setObstacle(gameState.mousePosition, false);
		}
	}
}

void mouseGameEvents(GameState gameState)
{
	bool onStack = (gameState.game.level.stack.isOnStack(gameState.mousePosition));
	if (gameState.event.type == sf::Event::MouseButtonPressed) {
		if (onStack) {
			gameState.game.level.event = gameState.game.level.dragStackObject(gameState.drag, gameState.mousePosition);
		} else {
			gameState.game.level.event = gameState.game.level.dragObject(gameState.drag, gameState.mousePosition);
		}
	}

	if (gameState.event.type == sf::Event::MouseButtonReleased) {
		if (onStack) {
			if (!gameState.drag.position.isNull()) {
				if (!gameState.drag.fromStack) {
					gameState.game.level.event = gameState.game.level.moveToStack(gameState.drag.position, gameState.mousePosition);
				} else {
					gameState.game.level.event = gameState.game.level.moveFromStackToStack(gameState.drag.position, gameState.mousePosition);
				}

				if (gameState.drag.position == gameState.mousePosition - gameState.game.level.stack.offset) {
					gameState.game.level.event = gameState.game.level.rotateObject(gameState.mousePosition);
				}
			}
		} else {
			if (!gameState.drag.fromStack) {
				if (!gameState.drag.position.isNull()) {
					gameState.game.level.event = gameState.game.level.moveObject(gameState.drag.position, gameState.mousePosition);
				}

				if (gameState.drag.position == gameState.mousePosition) {
					gameState.game.level.event = gameState.game.level.rotateObject(gameState.mousePosition);
				}
			} else {
				if (!gameState.drag.position.isNull()) {
					gameState.game.level.event = gameState.game.level.moveFromStack(gameState.drag.position, gameState.mousePosition);
				}
			}
		}

		gameState.drag.position.setNull();
	}
}

void clearLevel(GameState gameState)
{
	gameState.drag.position.setNull();
	gameState.game.clearLevel();
	gameState.game.level.event = true;
}

void resetLevel(GameState gameState)
{
	gameState.drag.position.setNull();
	gameState.game.resetLevel();
	gameState.game.level.event = true;
}

Object::Position floatToPosition(sf::Vector2f vector)
{
	return shortToPosition(static_cast<short>(vector.x / TILE_SIZE - OFFSET_X), static_cast<short>(vector.y / TILE_SIZE - OFFSET_Y));
}

Object::Position floatToPosition(float x, float y)
{
	return floatToPosition(sf::Vector2f(x, y));
}

Object::Position shortToPosition(short x, short y)
{
	Object::Position position;
	position.setPosition(x, y);
	return position;
}

sf::Vector2f positionToFloat(Object::Position position)
{
	return sf::Vector2f((position.getX() + OFFSET_X) * TILE_SIZE, (position.getY() + OFFSET_Y) * TILE_SIZE);
}

sf::Vector2f positionToFloat(short x, short y)
{
	return sf::Vector2f((x + OFFSET_X) * TILE_SIZE, (y + OFFSET_Y) * TILE_SIZE);
}