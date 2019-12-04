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

void deleteGameObjects(Game &game)
{
	for (size_t type = 0; type < OBJ_COUNT; ++type) {
		for (size_t index = 0; index < game.level.objectList[type].size(); ++index) {
			delete game.level.objectList[type][index];
		}
	}
}

void draw(Game &game, sf::RenderWindow &window, Drag &drag, Object::Position mousePosition)
{
	window.clear();

	drawBoard(game, window, mousePosition);
	drawStack(game, window);
	drawLasers(game, window);
	drawGameObject(game, window, drag, mousePosition);

	window.display();
}

void drawBoard(Game &game, sf::RenderWindow &window, Object::Position mousePosition)
{
	for (short y = 0; y < game.level.height; ++y) {
		for (short x = 0; x < game.level.width; ++x) {
			sf::Color outlineColor, fillColor;
			Object::Position currentPosition;
			currentPosition.setPosition(x, y);
			if (currentPosition == mousePosition) {
				outlineColor = (game.level.obstacles[currentPosition] ? lgray : dgray);
				fillColor = (game.level.obstacles[currentPosition] ? yellow : lgray);
			} else {
				outlineColor = (game.level.obstacles[currentPosition] ? lgray : dgray);
				fillColor = (game.level.obstacles[currentPosition] ? dgray : gray);
			}

			drawTile(window, currentPosition, outlineColor, fillColor);
		}
	}
}

void drawLasers(Game &game, sf::RenderWindow &window)
{
	drawLasers(game, window, true);
	drawLasers(game, window, false);
}

void drawLasers(Game &game, sf::RenderWindow &window, bool blackLasers)
{
	for (size_t beamerIndex = 0; beamerIndex < game.level.objectList[OBJ_BEAMER].size(); ++beamerIndex) {
		Beamer* beamer = (Beamer*) game.level.objectList[OBJ_BEAMER][beamerIndex];
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

			window.draw(vertices, size, sf::LinesStrip, (blackLasers ? sf::BlendNone : sf::BlendAdd));
		}
	}
}

void drawGameObject(Game &game, sf::RenderWindow &window, Drag &drag, Object::Position mousePosition)
{
	bool shiftPressed = (sf::Keyboard::isKeyPressed(sf::Keyboard::LShift));
	for (size_t type = 0; type < OBJ_COUNT; ++type) {
		for (size_t index = 0; index < game.level.objectList[type].size(); ++index) {
			// Draw the sprite unless it's dragged
			if (drag.position != game.level.objectList[type][index]->position) {
				window.draw(game.level.objectList[type][index]->sprite);
			}
		}
	}
	if (game.editor.isActive() && !shiftPressed) {
		if (game.editor.mode == ED_ADD_OR_REMOVE_OBJECTS) {
			game.editor.sprite.setPosition(mousePosition);
			window.draw(game.editor.sprite);
		}
	} else {
		// Draw the dragged element (if there is any)
		if (!drag.position.isNull()) {
			drag.sprite.setPosition(mousePosition);
			window.draw(drag.sprite);
		}
	}
}

void drawStack(Game &game, sf::RenderWindow &window)
{
	for (unsigned short y = 0; y < game.level.stack.height; ++y) {
		for (unsigned short x = 0; x < game.level.stack.width; ++x) {
			Object::Position currentPosition;
			currentPosition.setPosition(x, y);

			drawTile(window, game.level.stack.offset + currentPosition, dgray, gray);
		}
	}
}

void drawTile(sf::RenderWindow &window, Object::Position position, sf::Color outlineColor, sf::Color fillColor)
{
	sf::Vector2f vector = positionToFloat(position);
	window.draw(rectangleCreate(vector.x, vector.y, TILE_SIZE, TILE_SIZE, outlineColor));
	window.draw(rectangleCreate(vector.x + OUTLINE_SIZE, vector.y + OUTLINE_SIZE, TILE_SIZE - 2 * OUTLINE_SIZE, TILE_SIZE - 2 * OUTLINE_SIZE, fillColor));
}

void gameEvents(Game &game, Ev &event, Drag &drag, Object::Position position)
{
	keyboardGlobalEvents(game, event);

	bool shiftPressed = (sf::Keyboard::isKeyPressed(sf::Keyboard::LShift));
	if (game.editor.isActive() && !shiftPressed) {
		keyboardEditorEvents(game, event);
		mouseEditorEvents(game, event, position);
	} else {
		mouseGameEvents(game, event, drag, position);
	}

	if (game.level.event) {
		game.calculateLasers();
		game.level.event = false;
	}
}

void keyboardGlobalEvents(Game &game, Ev &event)
{
	if (event.type == sf::Event::KeyPressed) {
		switch (event.key.code) {
		case Key::E: {
			game.editor.turn(!game.editor.isActive());
			break;
		}
		case Key::S: {
			if (game.editor.isActive()) {
				game.saveLevel("001");
			}
			break;
		}
		default:
			break;
		}
	}
}

void keyboardEditorEvents(Game &game, Ev &event)
{
	if (event.type == sf::Event::KeyPressed) {
		switch (event.key.code) {
		case Key::Tab: {
			game.editor.switchMode();
			break;
		}
		case Key::Num1: {
			game.editor.setObject(OBJ_BEAMER);
			break;
		}
		case Key::Num2: {
			game.editor.setObject(OBJ_DOT);
			break;
		}
		case Key::Num3: {
			game.editor.setObject(OBJ_MIRROR);
			break;
		}
		case Key::Num4: {
			game.editor.setObject(OBJ_BENDER);
			break;
		}
		default:
			break;
		}
	}
}

void mouseEditorEvents(Game &game, Ev &event, Object::Position mousePosition)
{
	if (game.editor.mode == ED_EDIT_OBJECTS) {
		if (event.type == sf::Event::MouseButtonPressed) {
			if (event.mouseButton.button == sf::Mouse::Left) {
				game.level.event = game.level.rotateObject(mousePosition);
			} else if (event.mouseButton.button == sf::Mouse::Right) {
				game.level.event = game.level.changeObjectColor(mousePosition);
			}
		}
	} else if (game.editor.mode == ED_ADD_OR_REMOVE_OBJECTS) {
		if (event.type == sf::Event::MouseButtonPressed) {
			if (event.mouseButton.button == sf::Mouse::Left) {
				game.level.event = game.level.addObject(mousePosition, game.editor.getObject());
			} else if (event.mouseButton.button == sf::Mouse::Right) {
				game.level.event = game.level.removeObject(mousePosition);
			}
		}
	} else if (game.editor.mode == ED_ADD_OR_REMOVE_OBSTACLES) {
		if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
			game.level.event = game.level.setObstacle(mousePosition, true);
		} else if (sf::Mouse::isButtonPressed(sf::Mouse::Right)) {
			game.level.event = game.level.setObstacle(mousePosition, false);
		}
	}
}

void mouseGameEvents(Game &game, Ev &event, Drag &drag, Object::Position mousePosition)
{
	if (event.type == sf::Event::MouseButtonPressed) {
		game.level.event = game.level.dragObject(drag, mousePosition);
	}

	if (event.type == sf::Event::MouseButtonReleased) {
		// If the drag position is not null, move an object to the new location (if possible)
		if (!drag.position.isNull()) {
			game.level.event = game.level.moveObject(drag.position, mousePosition);
		}

		// Rotate an object if possible
		if (drag.position == mousePosition) {
			game.level.event = game.level.rotateObject(mousePosition);
		}

		drag.position.setNull();
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