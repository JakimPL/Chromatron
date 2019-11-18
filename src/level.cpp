#include <fstream>

#include "auxiliary.h"
#include "level.h"

Level::Level(Game* gm, const std::string &id)
{
	///TODO: error handling
	game = gm;

	std::string location = game->PATH_DATA + game->PATH_LEV_PREFIX + id + game->PATH_LEV_SUFFIX;
	std::ifstream levelFile(location, std::ios::binary | std::ios::in);
	if (levelFile.good()) {
		// Read level dimensions
		readByte(&levelFile, width);
		readByte(&levelFile, height);

		// Resize obstacle map
		obstacles.resize(height, std::vector<bool>(width));
		for (short y = 0; y < height; ++y) {
			for (short x = 0; x < width; ++x) {
				bool temp;
				levelFile.read((char*)&temp, 1);
				obstacles[x][y] = temp;
			}
		}

		// Read objects data
		unsigned short objectsCount;
		readByte(&levelFile, objectsCount);

		for (unsigned short obj = 0; obj < objectsCount; ++obj) {
			// Read the object's type
			unsigned short id;
			readByte(&levelFile, id);

			// Read object's position
			unsigned short x, y;
			readByte(&levelFile, x);
			readByte(&levelFile, y);

			// Add an object
			if (id == OBJ_BEAMER) {
				unsigned short red, green, blue;
				unsigned short direction;

				readByte(&levelFile, red);
				readByte(&levelFile, green);
				readByte(&levelFile, blue);
				readByte(&levelFile, direction);

				Color color(red > 0, green > 0, blue > 0);

				Beamer *beamer = new Beamer(game, x, y, color, direction);
				objectList.push_back(beamer);
			} else if (id == OBJ_DOT) {
				unsigned short red, green, blue;

				readByte(&levelFile, red);
				readByte(&levelFile, green);
				readByte(&levelFile, blue);

				Color color(red > 0, green > 0, blue > 0);

				Dot *dot = new Dot(game, x, y, color);
				objectList.push_back(dot);
			} else if (id == OBJ_MIRROR) {
				unsigned short direction;

				readByte(&levelFile, direction);

				Mirror *mirror = new Mirror(game, x, y, direction);
				objectList.push_back(mirror);
			} else if (id == OBJ_BENDER) {
				unsigned short direction;

				readByte(&levelFile, direction);

				Bender *bender = new Bender(game, x, y, direction);
				objectList.push_back(bender);
			}
		}
	} else {
		throw std::runtime_error("failed to load " + location + " file");
	}
}