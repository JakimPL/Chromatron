#include "game.h"

Game::Game()
{
	for (size_t index = 0; index < OBJ_COUNT; ++index) {
		textures.push_back(loadTexture(IMG_NAMES[index]));
	}
}

Game::~Game()
{
	for (size_t index = 0; index < textures.size(); ++index) {
		delete textures[index];
	}
}

// Load a sprite
sf::Texture* Game::loadTexture(const std::string &filename)
{
	std::string location = PATH_DATA + PATH_IMG_PREFIX + filename + PATH_IMG_SUFFIX;
	sf::Texture* image = new sf::Texture();
	if (!image->loadFromFile(location)) {
		throw std::runtime_error("failed to load " + location + " image file");
	}

	/*sf::Sprite sprite;
	sprite.setTexture(image);*/

	return image;
}