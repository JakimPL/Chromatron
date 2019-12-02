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

	// Game main loop
	bool gameEvent = true;
	Object::Position nullPosition = NULLPOS;
	Object::Position dragPosition = nullPosition;
	sf::Sprite dragSprite;

	while (window.isOpen()) {
		Ev event;
		while (window.pollEvent(event)) {
			if (event.type == Ev::Closed or event.key.code == Key::Escape) {
				window.close();
			}

			// Game events: mouse
			sf::Vector2f mousePosition =  window.mapPixelToCoords(sf::Mouse::getPosition(window));
			for (short y = 0; y < game.level.height; ++y) {
				for (short x = 0; x < game.level.width; ++x) {
					if (isMouseOn(x, y, mousePosition)) {
						// If mouse buttion is pressed
						if (event.type == sf::Event::MouseButtonPressed) {
							if (game.level.objectMap[x][y] != nullptr) {
								if (game.level.objectMap[x][y]->movable) {
									dragPosition = {x, y};
									dragSprite = game.level.objectMap[x][y]->sprite;
								}
							}
						}
						if (event.type == sf::Event::MouseButtonReleased) {
							dragPosition = nullPosition;
							if (game.level.objectMap[x][y] != nullptr) {
								game.level.objectMap[x][y]->rotate(event.mouseButton.button == sf::Mouse::Left);
								gameEvent = true;
							}
						}
					}
				}
			}

			window.clear();

			if (gameEvent) {
				game.calculateLasers();
				gameEvent = false;
			}

			// Draw the board
			for (short y = 0; y < game.level.height; ++y) {
				for (short x = 0; x < game.level.width; ++x) {
					sf::Color outlineColor, fillColor;
					if (isMouseOn(x, y, mousePosition)) {
						outlineColor = (game.level.obstacles[x][y] ? lgray : dgray);
						fillColor = (game.level.obstacles[x][y] ? yellow : lgray);
					} else {
						outlineColor = (game.level.obstacles[x][y] ? lgray : dgray);
						fillColor = (game.level.obstacles[x][y] ? dgray : gray);
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
