#include <iostream>

#include "main.h"

int main(int argc, char* argv[])
{
	// Handle application's parameters
	for (int arg = 0; arg < argc; ++arg) {
		///TODO: Handle application's parameters
	}

	// Create a game object
	Game game;

	// Initialize the window
	sf::RenderWindow window(sf::VideoMode(SCREEN_WIDTH, SCREEN_HEIGHT), "Chromatron");
	sf::View view(sf::FloatRect(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT));
	window.setView(view);

	// Load a level
	game.loadLevel("000");

	Object::Position nullPosition = NULLPOSITION;
	Object::Position dragPosition = nullPosition;
	sf::Sprite dragSprite;

	// Game main loop
	bool gameEvent = true;
	while (window.isOpen()) {
		Ev event;
		while (window.pollEvent(event)) {
			if (event.type == Ev::Closed or event.key.code == Key::Escape) {
				window.close();
			}

			// Game events: mouse
			sf::Vector2f mousePositionVector = window.mapPixelToCoords(sf::Mouse::getPosition(window));
			Object::Position mousePosition = Object::Position::createPosition(mousePositionVector);
			Object* object = game.level.objectMap[mousePosition];

			if (event.type == sf::Event::MouseButtonPressed) {
				if (object != nullptr) {
					if (object->movable) {
						dragPosition = mousePosition;
						dragSprite = object->sprite;
					}
				}
			}

			if (event.type == sf::Event::MouseButtonReleased) {
				// If the drag position is not null, move an object to the new location (if possible)
				if (dragPosition != nullPosition) {
					gameEvent = game.level.moveObject(dragPosition, mousePosition);
				}

				// Rotate an object if possible
				if (dragPosition == mousePosition || dragPosition == nullPosition) {
					if (object != nullptr) {
						object->rotate(event.mouseButton.button == sf::Mouse::Right);
						gameEvent = true;
					}
				}

				dragPosition = nullPosition;
			}


			// Update game status: calculate lasers
			if (gameEvent) {
				game.calculateLasers();
				gameEvent = false;
			}

			window.clear();

			// Draw the board
			for (short y = 0; y < game.level.height; ++y) {
				for (short x = 0; x < game.level.width; ++x) {
					sf::Color outlineColor, fillColor;
					Object::Position currentPosition = {x, y};
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

			// Draw lasers
			for (size_t beamerIndex = 0; beamerIndex < game.level.objectList[OBJ_BEAMER].size(); ++beamerIndex) {
				Beamer* beamer = (Beamer*) game.level.objectList[OBJ_BEAMER][beamerIndex];
				size_t rays = beamer->laser.size();
				for (size_t rayIndex = 0; rayIndex < rays; ++rayIndex) {
					size_t size = beamer->laser[rayIndex].size();
					sf::Vertex vertices[size];
					std::copy(beamer->laser[rayIndex].begin(), beamer->laser[rayIndex].end(), vertices);
					window.draw(vertices, size, sf::LinesStrip);
				}
			}

			// Draw game objects
			for (size_t type = 0; type < OBJ_COUNT; ++type) {
				for (size_t index = 0; index < game.level.objectList[type].size(); ++index) {
					// Draw the sprite unless it's dragged
					if (dragPosition != game.level.objectList[type][index]->position) {
						window.draw(game.level.objectList[type][index]->sprite);
					}
				}
			}

			// Draw the dragged element (if there is any)
			if (dragPosition != nullPosition) {
				dragSprite.setPosition(mousePosition);
				window.draw(dragSprite);
			}

			window.display();
		}
	}

	// Delete level objects
	for (size_t type = 0; type < OBJ_COUNT; ++type) {
		for (size_t index = 0; index < game.level.objectList[type].size(); ++index) {
			delete game.level.objectList[type][index];
		}
	}

	return 0;
}
