#include <iostream>

#include "main.h"

int main(int argc, char *argv[])
{
	// Handle application's parameters
	for (int arg = 0; arg < argc; ++arg) {
		///TODO: Handle application's parameters
	}

	// Create a game object
	Game game;

	// Initialize the window
	sf::RenderWindow window(sf::VideoMode(game.SCREEN_WIDTH, game.SCREEN_HEIGHT), "Chromatron");

	// Load a level
	game.loadLevel("000");

	bool gameEvent = true;

	// Game main loop
	while (window.isOpen()) {
		Ev event;
		while (window.pollEvent(event)) {
			if (event.type == Ev::Closed or event.key.code == Key::Escape) {
				window.close();
			}

			window.clear();

			if (gameEvent) {
				game.calculateLasers();
				gameEvent = false;
			}

			// Draw the board
			for (short y = 0; y < game.level.height; ++y) {
				for (short x = 0; x < game.level.width; ++x) {
					sf::Color outlineColor = (game.level.obstacles[x][y] ? lgray : dgray);
					sf::Color fillColor = (game.level.obstacles[x][y] ? dgray : gray);
					window.draw(rectangleCreate(game.OFFSET_X + game.TILE_SIZE * x, game.OFFSET_Y + game.TILE_SIZE * y, game.TILE_SIZE, game.TILE_SIZE, outlineColor));
					window.draw(rectangleCreate(game.OFFSET_X + game.TILE_SIZE * x + game.OUTLINE_SIZE, game.OFFSET_Y + game.TILE_SIZE * y + game.OUTLINE_SIZE, game.TILE_SIZE - 2 * game.OUTLINE_SIZE, game.TILE_SIZE - 2 * game.OUTLINE_SIZE, fillColor));
				}
			}


			// Draw lasers
			for (size_t beamer = 0; beamer < game.level.beamerList.size(); ++beamer) {
				for (size_t ray = 0; ray < game.level.beamerList[beamer]->laser.size(); ++ray) {
					sf::Vertex line[2];
					line[0].position = sf::Vector2f(game.level.beamerList[beamer]->laser[ray].x_start, game.level.beamerList[beamer]->laser[ray].y_start);
					line[1].position = sf::Vector2f(game.level.beamerList[beamer]->laser[ray].x_end, game.level.beamerList[beamer]->laser[ray].y_end);
					line[0].color = line[1].color = game.level.beamerList[beamer]->laser[ray].color.convertToColor();
					window.draw(line, 2, sf::Lines);
				}
			}

			// Draw game objects
			for (size_t index = 0; index < game.level.objectList.size(); ++index) {
				// Draw the sprite
				window.draw(game.level.objectList[index]->sprite);
			}

			window.display();
		}
	}

	// Delete level objects
	for (size_t index = 0; index < game.level.objectList.size(); ++index) {
		delete game.level.objectList[index];
	}

	return 0;
}
