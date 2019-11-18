#include "game.h"

Game::Game()
{
	textures.push_back(loadTexture("empty"));
	textures.push_back(loadTexture("beamer"));
	textures.push_back(loadTexture("dot"));
	textures.push_back(loadTexture("mirror"));
	textures.push_back(loadTexture("bender"));
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