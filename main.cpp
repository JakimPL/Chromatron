#include <iostream>

#include "main.h"

bool isMouseOn(short x, short y, sf::Vector2f mousePosition)
{
	return (mousePosition.x >= OFFSET_X + TILE_SIZE * x && mousePosition.x < OFFSET_X + TILE_SIZE * (x + 1) && mousePosition.y >= OFFSET_Y + TILE_SIZE * y && mousePosition.y < OFFSET_Y + TILE_SIZE * (y + 1));
}

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

	bool gameEvent = true;
	bool mouseClicked = false;
	// Game main loop
	while (window.isOpen()) {
		Ev event;
		while (window.pollEvent(event)) {
			if (event.type == Ev::Closed or event.key.code == Key::Escape) {
				window.close();
			}

			window.clear();

			// Draw the board
			sf::Vector2f mousePosition =  window.mapPixelToCoords(sf::Mouse::getPosition(window));
			for (short y = 0; y < game.level.height; ++y) {
				for (short x = 0; x < game.level.width; ++x) {
					sf::Color outlineColor, fillColor;
					if (isMouseOn(x, y, mousePosition)) {
						outlineColor = (game.level.obstacles[x][y] ? lgray : dgray);
						fillColor = (game.level.obstacles[x][y] ? yellow : lgray);

						// If LMB is pressed
						if (event.type == sf::Event::MouseButtonPressed) {
							if (game.level.objectMap[x][y] != NULL) {
								game.level.objectMap[x][y]->rotate(event.mouseButton.button == sf::Mouse::Left);
								gameEvent = true;
							}
						}
					} else {
						outlineColor = (game.level.obstacles[x][y] ? lgray : dgray);
						fillColor = (game.level.obstacles[x][y] ? dgray : gray);
					}

					window.draw(rectangleCreate(OFFSET_X + TILE_SIZE * x, OFFSET_Y + TILE_SIZE * y, TILE_SIZE, TILE_SIZE, outlineColor));
					window.draw(rectangleCreate(OFFSET_X + TILE_SIZE * x + OUTLINE_SIZE, OFFSET_Y + TILE_SIZE * y + OUTLINE_SIZE, TILE_SIZE - 2 * OUTLINE_SIZE, TILE_SIZE - 2 * OUTLINE_SIZE, fillColor));
				}
			}

			if (gameEvent) {
				game.calculateLasers();
				gameEvent = false;
			}

			// Draw lasers
			for (size_t beamerIndex = 0; beamerIndex < game.level.objectList[OBJ_BEAMER].size(); ++beamerIndex) {
				Beamer* beamer = (Beamer*) game.level.objectList[OBJ_BEAMER][beamerIndex];
				for (size_t rayIndex = 0; rayIndex < beamer->laser.size(); ++rayIndex) {
					Ray ray = beamer->laser[rayIndex];
					sf::Vertex line[2];
					line[0].position = sf::Vector2f(ray.x_start, ray.y_start);
					line[1].position = sf::Vector2f(ray.x_end, ray.y_end);
					line[0].color = line[1].color = ray.color.convertToColor();
					window.draw(line, 2, sf::Lines);
				}
			}

			// Draw game objects
			for (size_t type = 0; type < OBJ_COUNT; ++type) {
				for (size_t index = 0; index < game.level.objectList[type].size(); ++index) {
					// Draw the sprite
					window.draw(game.level.objectList[type][index]->sprite);
				}
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
