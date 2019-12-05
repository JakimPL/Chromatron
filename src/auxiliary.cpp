#include "auxiliary.h"

sf::RectangleShape rectangleCreate(int x, int y, int w, int h, sf::Color color)
{
	sf::RectangleShape rectangle(sf::Vector2f(w, h));
	rectangle.setPosition(x, y);
	rectangle.setFillColor(color);
	return rectangle;
}

void readByte(std::ifstream* file, unsigned short &var)
{
	char buffer;
	file->read(&buffer, 1);
	var = static_cast<unsigned short>(buffer);
}

void writeByte(std::ofstream* file, unsigned short var)
{
	char buffer = static_cast<char>(var);
	file->write(&buffer, 1);
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
	gameState.game.loadLevel("000");
}

void mainLoop(GameState gameState)
{
	while (gameState.window.isOpen()) {
		gameState.update();
		while (gameState.window.pollEvent(gameState.event)) {
			if (gameState.event.type == sf::Event::Closed or gameState.event.key.code == sf::Keyboard::Escape) {
				gameState.window.close();
			}

			gameEvents(gameState);
			draw(gameState);
		}
	}
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
	drawGameObject(gameState);

	gameState.window.display();
}

void drawBoard(GameState gameState)
{
	for (short y = 0; y < gameState.game.level.height; ++y) {
		for (short x = 0; x < gameState.game.level.width; ++x) {
			sf::Color outlineColor, fillColor;
			Object::Position currentPosition;
			currentPosition.setPosition(x, y);
			if (currentPosition == gameState.mousePosition) {
				outlineColor = (gameState.game.level.obstacles[currentPosition] ? lgray : dgray);
				fillColor = (gameState.game.level.obstacles[currentPosition] ? yellow : lgray);
			} else {
				outlineColor = (gameState.game.level.obstacles[currentPosition] ? lgray : dgray);
				fillColor = (gameState.game.level.obstacles[currentPosition] ? dgray : gray);
			}

			drawTile(gameState, currentPosition, outlineColor, fillColor);
		}
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

void drawGameObject(GameState gameState)
{
	bool shiftPressed = (sf::Keyboard::isKeyPressed(sf::Keyboard::LShift));
	for (size_t type = 0; type < OBJ_COUNT; ++type) {
		for (size_t index = 0; index < gameState.game.level.objectList[type].size(); ++index) {
			// Draw the sprite unless it's dragged
			if (gameState.drag.position != gameState.game.level.objectList[type][index]->position) {
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
		// Draw the dragged element (if there is any)
		if (!gameState.drag.position.isNull()) {
			gameState.drag.sprite.setPosition(gameState.mousePosition);
			gameState.window.draw(gameState.drag.sprite);
		}
	}
}

void drawStack(GameState gameState)
{
	for (short y = 0; y < gameState.game.level.stack.height; ++y) {
		for (short x = 0; x < gameState.game.level.stack.width; ++x) {
			Object::Position currentPosition;
			currentPosition.setPosition(x, y);

			drawTile(gameState, gameState.game.level.stack.offset + currentPosition, dgray, gray);
		}
	}
}

void drawTile(GameState gameState, Object::Position position, sf::Color outlineColor, sf::Color fillColor)
{
	sf::Vector2f vector = positionToFloat(position);
	gameState.window.draw(rectangleCreate(vector.x, vector.y, TILE_SIZE, TILE_SIZE, outlineColor));
	gameState.window.draw(rectangleCreate(vector.x + OUTLINE_SIZE, vector.y + OUTLINE_SIZE, TILE_SIZE - 2 * OUTLINE_SIZE, TILE_SIZE - 2 * OUTLINE_SIZE, fillColor));
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
		gameState.game.level.event = false;
	}
}

void keyboardGlobalEvents(GameState gameState)
{
	if (gameState.event.type == sf::Event::KeyPressed) {
		switch (gameState.event.key.code) {
		case sf::Keyboard::E: {
			gameState.game.editor.turn(!gameState.game.editor.isActive());
			break;
		}
		case sf::Keyboard::S: {
			if (gameState.game.editor.isActive()) {
				gameState.game.saveLevel("001");
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
	if (gameState.event.type == sf::Event::MouseButtonPressed) {
		gameState.game.level.event = gameState.game.level.dragObject(gameState.drag, gameState.mousePosition);
	}

	if (gameState.event.type == sf::Event::MouseButtonReleased) {
		if (!gameState.drag.position.isNull()) {
			gameState.game.level.event = gameState.game.level.moveObject(gameState.drag.position, gameState.mousePosition);
		}

		if (gameState.drag.position == gameState.mousePosition) {
			gameState.game.level.event = gameState.game.level.rotateObject(gameState.mousePosition);
		}

		gameState.drag.position.setNull();
	}
}

Object::Position floatToPosition(sf::Vector2f vector)
{
	Object::Position position;
	position.setPosition(static_cast<short>(vector.x / TILE_SIZE - OFFSET_X), static_cast<short>(vector.y / TILE_SIZE - OFFSET_Y));
	return position;
}

Object::Position floatToPosition(float x, float y)
{
	return floatToPosition(sf::Vector2f(x, y));
}

sf::Vector2f positionToFloat(Object::Position position)
{
	return sf::Vector2f((position.getX() + OFFSET_X) * TILE_SIZE, (position.getY() + OFFSET_Y) * TILE_SIZE);
}

sf::Vector2f positionToFloat(short x, short y)
{
	return sf::Vector2f((x + OFFSET_X) * TILE_SIZE, (y + OFFSET_Y) * TILE_SIZE);
}