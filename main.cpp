#include <iostream>

#include "main.h"

void deleteGameObjects(Game &game)
{
	for (size_t type = 0; type < OBJ_COUNT; ++type) {
		for (size_t index = 0; index < game.level.objectList[type].size(); ++index) {
			delete game.level.objectList[type][index];
		}
	}
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

			window.draw(rectangleCreate(OFFSET_X + TILE_SIZE * x, OFFSET_Y + TILE_SIZE * y, TILE_SIZE, TILE_SIZE, outlineColor));
			window.draw(rectangleCreate(OFFSET_X + TILE_SIZE * x + OUTLINE_SIZE, OFFSET_Y + TILE_SIZE * y + OUTLINE_SIZE, TILE_SIZE - 2 * OUTLINE_SIZE, TILE_SIZE - 2 * OUTLINE_SIZE, fillColor));
		}
	}
}

void drawLasers(Game &game, sf::RenderWindow &window, bool blackLasers = false)
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
		if (!game.editor.editMode) {
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

void gameEvents(Game &game)
{
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
	if (game.editor.editMode) {
		if (event.type == sf::Event::MouseButtonPressed) {
			if (event.mouseButton.button == sf::Mouse::Left) {
				game.level.event = game.level.rotateObject(mousePosition);
			} else if (event.mouseButton.button == sf::Mouse::Right) {
				game.level.event = game.level.changeObjectColor(mousePosition);
			}
		}
	} else {
		if (event.type == sf::Event::MouseButtonPressed) {
			if (event.mouseButton.button == sf::Mouse::Left) {
				game.level.event = game.level.addObject(game.editor.getObject(), mousePosition);
			} else if (event.mouseButton.button == sf::Mouse::Right) {
				game.level.event = game.level.removeObject(mousePosition);
			}
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

int main(int argc, char* argv[])
{
	// Handle application's parameters
	bool editorOn = false;
	for (int arg = 0; arg < argc; ++arg) {
		editorOn = std::string(argv[arg]) == "--editor";
	}

	// Create a game object
	Game game;
	Drag drag;

	// Initialize the window
	sf::RenderWindow window(sf::VideoMode(SCREEN_WIDTH, SCREEN_HEIGHT), "Chromatron");
	sf::View view(sf::FloatRect(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT));
	window.setView(view);

	// Load a level
	game.loadLevel("000");
	game.editor.turn(editorOn);

	while (window.isOpen()) {
		Ev event;
		while (window.pollEvent(event)) {
			if (event.type == Ev::Closed or event.key.code == Key::Escape) {
				window.close();
			}

			bool shiftPressed = (sf::Keyboard::isKeyPressed(sf::Keyboard::LShift));
			sf::Vector2f mousePositionVector = window.mapPixelToCoords(sf::Mouse::getPosition(window));
			Object::Position mousePosition = Object::Position::createPosition(mousePositionVector);

			keyboardGlobalEvents(game, event);

			if (game.editor.isActive() && !shiftPressed) {
				keyboardEditorEvents(game, event);
				mouseEditorEvents(game, event, mousePosition);
			} else {
				mouseGameEvents(game, event, drag, mousePosition);
			}

			gameEvents(game);
			window.clear();

			drawBoard(game, window, mousePosition);
			drawLasers(game, window, true);
			drawLasers(game, window);
			drawGameObject(game, window, drag, mousePosition);

			window.display();
		}
	}

	// Delete level objects
	deleteGameObjects(game);

	return 0;
}
