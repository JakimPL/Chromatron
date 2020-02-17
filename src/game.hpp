#ifndef GAME_H
#define GAME_H

#include <vector>
#include <map>
#include <string>

#include <SFML/Graphics.hpp>
#include "constants.hpp"
#include "drag.hpp"
#include "editor.hpp"
#include "graphics.hpp"
#include "level.hpp"
#include "levelset.hpp"
#include "stack.hpp"

class Object;

class Game
{
public:
	Game();
	~Game();
	struct Level level;
	struct LevelSet levelSet;
	struct Editor editor;

	std::string levelId;
	Graphics graphics;
	Drag drag;
};

#endif
