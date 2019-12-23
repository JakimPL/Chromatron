#include <fstream>
#include <SFML/Graphics.hpp>
#include "graphics.hpp"
#include "constants.hpp"

sf::Texture* Graphics::loadTexture(const std::string &filename)
{
	std::string location = PATH_DATA + PATH_IMG_PREFIX + filename + PATH_IMG_SUFFIX;
	sf::Texture* image = new sf::Texture();
	if (!image->loadFromFile(location)) {
		throw std::runtime_error("failed to load " + location + " image file");
	}

	return image;
}

void Graphics::loadTextures()
{
	for (size_t index = 0; index < OBJ_COUNT + 2; ++index) {
		textures.push_back(loadTexture(IMG_OBJECT_NAMES[index]));
	}

	for (size_t index = 0; index < TIL_COUNT; ++index) {
		tiles.push_back(loadTexture(IMG_TILE_NAMES[index]));
	}
}

void Graphics::deleteTextures()
{
	for (size_t index = 0; index < textures.size(); ++index) {
		delete textures[index];
	}

	for (size_t index = 0; index < tiles.size(); ++index) {
		delete tiles[index];
	}
}