#include <iostream>

#include "gamestate.hpp"
#include "auxiliary.hpp"
#include "constants.hpp"
#include "log.hpp"

GameState::GameState(Game &gam, sf::RenderWindow &win, Drag &drg, sf::Event &ev) : game(gam), window(win), drag(drg), event(ev)
{

}

void GameState::GameState::update()
{
	mousePosition = floatToPosition(window.mapPixelToCoords(sf::Mouse::getPosition(window)));
}

void GameState::handleApplicationParameters(int argc, char* argv[])
{
	bool editorOn = false;
	game.levelSet.name = "Chromatron";
	for (int arg = 0; arg < argc; ++arg) {
		if (std::string(argv[arg]) == "--editor") {
			editorOn = std::string(argv[arg]) == "--editor";
		} else if (std::string(argv[arg]) == "--levelset") {
			arg++;
			if (arg >= argc) {
				LogError("No levelset name given; loading " + game.levelSet.name + " levelset");
			} else {
				std::string name = argv[arg];
				if (game.levelSet.checkSet(name)) {
					game.levelSet.name = name;
				} else {
					LogError("Levelset " + name + " does not exist");
				}
			}
		}
	}

	game.editor.turn(editorOn);
}

void GameState::initializeGame()
{
	LogNone("Game starts");
	window.setView(sf::View(sf::FloatRect(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT)));
	game.levelSet.loadSet(game.levelSet.name);
	game.graphics.loadFont();
	updateLevelsList();
}

void GameState::mainLoop()
{
	while (window.isOpen()) {
		update();
		while (window.pollEvent(event)) {
			if (event.type == sf::Event::Closed or sf::Keyboard::isKeyPressed(sf::Keyboard::Escape)) {
				window.close();
				break;
			}

			gameEvents();
			draw();
		}
	}
}

void GameState::endGame()
{
	game.levelSet.saveSet(game.levelSet.name);
	deleteGameObjects();
	LogNone("Game ends");
}

void GameState::deleteGameObjects()
{
	for (size_t type = 0; type < OBJ_COUNT; ++type) {
		for (size_t index = 0; index < game.level.objectList[type].size(); ++index) {
			delete game.level.objectList[type][index];
		}
	}
}

void GameState::draw()
{
	window.clear();

	drawBoard();
	drawStack();
	drawLasers();
	drawGameObjects();
	drawLevelsList();

	window.display();
}

void GameState::drawBoard()
{
	window.draw(rectangleCreate(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, (game.editor.isActive() ? lred : lgray)));

	for (short y = 0; y < game.level.height; ++y) {
		for (short x = 0; x < game.level.width; ++x) {
			Position currentPosition = shortToPosition(x, y);
			drawTile(currentPosition);
		}
	}

	if (!game.level.isOutsideBoard(mousePosition)) {
		drawSelector(mousePosition);
	}
}

void GameState::drawLasers()
{
	drawLasers(true);
	drawLasers(false);
}

void GameState::drawLasers(bool blackLasers)
{
	for (Object *object : game.level.objectList[OBJ_BEAMER]) {
		Beamer* beamer = (Beamer*) object;
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

			window.draw(vertices, size, sf::Lines, (blackLasers ? sf::BlendNone : sf::BlendAdd));
		}
	}
}

void GameState::drawLevelsList()
{
	for (unsigned short level = 0; level < texts.size(); ++level) {
		window.draw(texts[level]);
	}
}

void GameState::drawGameObjects()
{
	bool shiftPressed = (sf::Keyboard::isKeyPressed(sf::Keyboard::LShift));
	for (size_t type = 0; type < OBJ_COUNT; ++type) {
		for (size_t index = 0; index < game.level.objectList[type].size(); ++index) {
			if (drag.fromStack != game.level.objectList[type][index]->inStack or drag.position != game.level.objectList[type][index]->position) {
				if (game.level.objectList[type][index]->id == OBJ_BEAMER) {
					window.draw(game.level.objectList[type][index]->baseSprite);
				}
				window.draw(game.level.objectList[type][index]->sprite);
			}
		}
	}

	if (game.editor.isActive() and !shiftPressed) {
		if (game.editor.mode == ED_ADD_OR_REMOVE_OBJECTS) {
			game.editor.sprite.setPosition(mousePosition);
			window.draw(game.editor.sprite);
		}
	} else {
		if (!drag.position.isNull()) {
			drag.sprite.setPosition(mousePosition);
			window.draw(drag.sprite);
		}
	}
}

void GameState::drawSelector(Position mousePosition)
{
	sf::Color fillColor, outlineColor;
	bool outlineOnly = false;
	if (game.editor.isActive()) {
		switch (game.editor.mode) {
		case ED_EDIT_OBJECTS: {
			fillColor = dyellow;
			outlineColor = lgray;
			break;
		}
		case ED_ADD_OR_REMOVE_OBJECTS: {
			fillColor = dred;
			outlineColor = gray;
			break;
		}
		default:
			outlineColor = lgray;
			outlineOnly = true;
			break;
		}
	} else {
		fillColor = dyellow;
		outlineColor = lgray;
	}

	drawSelectorSquare(mousePosition, fillColor, outlineColor, outlineOnly);
}

void GameState::drawStack()
{
	for (short y = 0; y < game.level.stack.height; ++y) {
		for (short x = 0; x < game.level.stack.width; ++x) {
			Position currentPosition = shortToPosition(x, y);
			drawTile(currentPosition, true);
		}
	}

	if (game.level.stack.isOnStack(mousePosition)) {
		drawSelector(mousePosition);
	}
}

void GameState::drawTile(Position position, bool inStack)
{
	sf::Sprite sprite = (inStack ? game.level.stack.sprites[position] : game.level.tileSprites[position]);
	window.draw(sprite);
}

void GameState::drawSelectorSquare(Position position, sf::Color fillColor, sf::Color outlineColor, bool outlineOnly)
{
	sf::Vector2f realPosition(position);
	realPosition.x -= TILE_SIZE / 2;
	realPosition.y -= TILE_SIZE / 2;
	if (!outlineOnly) {
		window.draw(rectangleCreate(realPosition.x, realPosition.y, TILE_SIZE, TILE_SIZE, outlineColor));
		window.draw(rectangleCreate(realPosition.x + OUTLINE_SIZE, realPosition.y + OUTLINE_SIZE, TILE_SIZE - 2 * OUTLINE_SIZE, TILE_SIZE - 2 * OUTLINE_SIZE, fillColor));
	} else {
		sf::Vertex outline[] = {
			sf::Vertex(sf::Vector2f(realPosition) + sf::Vector2f(1, 1)),
			sf::Vertex(sf::Vector2f(realPosition) + sf::Vector2f(TILE_SIZE, 1)),
			sf::Vertex(sf::Vector2f(realPosition) + sf::Vector2f(TILE_SIZE, TILE_SIZE)),
			sf::Vertex(sf::Vector2f(realPosition) + sf::Vector2f(1, TILE_SIZE)),
			sf::Vertex(sf::Vector2f(realPosition))
		};

		for (size_t node = 0; node < 5; ++node) {
			outline[node].color = outlineColor;
		}

		window.draw(outline, 5, sf::LinesStrip);
	}
}

void GameState::gameEvents()
{
	keyboardGlobalEvents();

	bool shiftPressed = (sf::Keyboard::isKeyPressed(sf::Keyboard::LShift));
	if (game.editor.isActive() and !shiftPressed) {
		keyboardEditorEvents();
		mouseEditorEvents();
	} else {
		mouseGameEvents();
	}

	if (game.level.event) {
		game.level.calculateLasers();

		if (!game.editor.isActive()) {
			if (game.levelSet.levelStates[game.levelSet.getCurrentLevel() - 1] != LS_PASSED) {
				if (game.level.checkWin()) {
					game.levelSet.levelStates[game.levelSet.getCurrentLevel() - 1] = LS_PASSED;
					game.levelSet.unlockNextLevel();
				}
			}
		}

		game.level.event = false;
	}
}

sf::Color GameState::getTextColor(unsigned short level)
{
	if (level == game.levelSet.getCurrentLevel()) {
		return white;
	} else {
		LevelState levelState = game.levelSet.levelStates[level - 1];
		switch (levelState) {
		case LS_LOCKED:
			return red;
		case LS_AVAILABLE:
			return blue;
		case LS_PASSED:
			return green;
		}
	}

	return black;
}

void GameState::keyboardGlobalEvents()
{
	if (event.type == sf::Event::KeyPressed) {
		switch (event.key.code) {
		case sf::Keyboard::C: {
			if (game.editor.isActive()) {
				clearLevel();
			}
			break;
		}
		case sf::Keyboard::E: {
			game.editor.turn(!game.editor.isActive());
			break;
		}
		case sf::Keyboard::R: {
			resetLevel();
			break;
		}
		case sf::Keyboard::S: {
			saveLevel();
			break;
		}
		case sf::Keyboard::Space: {
			nextLevel();
			break;
		}
		case sf::Keyboard::BackSpace: {
			previousLevel();
			break;
		}
		default:
			break;
		}
	}
}

void GameState::keyboardEditorEvents()
{
	///TODO: abstraction
	if (event.type == sf::Event::KeyPressed) {
		switch (event.key.code) {
		case sf::Keyboard::Tab: {
			game.editor.switchMode();
			break;
		}
		case sf::Keyboard::Num1: {
			game.editor.setObject(OBJ_BEAMER);
			break;
		}
		case sf::Keyboard::Num2: {
			game.editor.setObject(OBJ_DOT);
			break;
		}
		case sf::Keyboard::Num3: {
			game.editor.setObject(OBJ_MIRROR);
			break;
		}
		case sf::Keyboard::Num4: {
			game.editor.setObject(OBJ_BENDER);
			break;
		}
		case sf::Keyboard::Num5: {
			game.editor.setObject(OBJ_SPLITTER);
			break;
		}
		case sf::Keyboard::Num6: {
			game.editor.setObject(OBJ_CONDUIT);
			break;
		}
		case sf::Keyboard::Num7: {
			game.editor.setObject(OBJ_FILTER);
			break;
		}
		case sf::Keyboard::Num8: {
			game.editor.setObject(OBJ_PRISM);
			break;
		}
		case sf::Keyboard::Num9: {
			game.editor.setObject(OBJ_DOPPLER);
			break;
		}
		case sf::Keyboard::Num0: {
			game.editor.setObject(OBJ_TANGLER);
			break;
		}
		case sf::Keyboard::Q: {
			game.editor.setObject(OBJ_TELEPORTER);
			break;
		}
		default:
			break;
		}
	}
}

void GameState::mouseEditorEvents()
{
	if (game.editor.mode == ED_EDIT_OBJECTS) {
		if (event.type == sf::Event::MouseButtonPressed) {
			if (event.mouseButton.button == sf::Mouse::Left) {
				game.level.event = game.level.rotateObject(mousePosition);
			} else if (event.mouseButton.button == sf::Mouse::Right) {
				game.level.event = game.level.changeObjectColor(mousePosition);
			}
		}
	} else if (game.editor.mode == ED_ADD_OR_REMOVE_OBJECTS) {
		if (event.type == sf::Event::MouseButtonPressed) {
			if (event.mouseButton.button == sf::Mouse::Left) {
				game.level.event = game.level.addObject(mousePosition, game.editor.getObject());
			} else if (event.mouseButton.button == sf::Mouse::Right) {
				game.level.event = game.level.removeObject(mousePosition);
			}
		}
	} else if (game.editor.mode == ED_ADD_OR_REMOVE_OBSTACLES) {
		if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
			game.level.event = game.level.setObstacle(mousePosition, true);
		} else if (sf::Mouse::isButtonPressed(sf::Mouse::Right)) {
			game.level.event = game.level.setObstacle(mousePosition, false);
		}
	}
}

void GameState::mouseGameEvents()
{
	bool onStack = (game.level.stack.isOnStack(mousePosition));
	if (event.type == sf::Event::MouseButtonPressed) {
		if (onStack) {
			game.level.event = game.level.dragStackObject(drag, mousePosition);
		} else {
			game.level.event = game.level.dragObject(drag, mousePosition);
		}
	}

	if (event.type == sf::Event::MouseButtonReleased) {
		if (onStack) {
			if (!drag.position.isNull()) {
				if (!drag.fromStack) {
					game.level.event = game.level.moveToStack(drag.position, mousePosition);
				} else {
					game.level.event = game.level.moveFromStackToStack(drag.position, mousePosition);
				}

				if (drag.position == mousePosition - game.level.stack.offset) {
					game.level.event = game.level.rotateObject(mousePosition);
				}
			}
		} else {
			if (!drag.fromStack) {
				if (!drag.position.isNull()) {
					game.level.event = game.level.moveObject(drag.position, mousePosition);
				}

				if (drag.position == mousePosition) {
					game.level.event = game.level.rotateObject(mousePosition, event.mouseButton.button);
				}
			} else {
				if (!drag.position.isNull()) {
					game.level.event = game.level.moveFromStack(drag.position, mousePosition);
				}
			}
		}

		drag.position.setNull();
	}
}

void GameState::clearLevel()
{
	drag.position.setNull();
	game.level.clearLevel();
	game.level.event = true;
	updateLevelsList();
}

void GameState::loadLevel()
{
	clearLevel();
	game.level.loadLevel(game.levelSet.getCurrentLevel());
	updateLevelsList();
}

void GameState::nextLevel()
{
	if (game.editor.isActive()) {
		if (game.levelSet.isLevelLast()) {
			game.levelSet.levels++;
			game.levelSet.nextLevel();
			game.levelSet.saveSet(false);
			clearLevel();
			LogInfo("Added a new level to the levelset");
			game.level.saveLevel(game.levelId);
		}
	} else {
		if (!game.levelSet.isLevelLast()) {
			if (game.levelSet.levelStates[game.levelSet.getCurrentLevel()] != LS_LOCKED) {
				saveCurrentLevel();
				game.levelSet.nextLevel();
				loadLevel();
			}
		}
	}
}

void GameState::previousLevel()
{
	if (!game.levelSet.isLevelFirst()) {
		saveCurrentLevel();
		game.levelSet.previousLevel();
		loadLevel();
	}
}

void GameState::resetLevel()
{
	drag.position.setNull();
	game.level.resetLevel(true);
	game.level.event = true;
}

void GameState::saveCurrentLevel()
{
	if (!game.editor.isActive()) {
		game.levelSet.saveCurrentLevel();
	}
}

void GameState::saveLevel()
{
	if (game.editor.isActive()) {
		drag.position.setNull();
		game.level.saveLevel(game.levelId);
		game.level.event = true;
	} else {
		game.levelSet.saveSet(game.levelSet.name);
	}
}


void GameState::updateLevelsList()
{
	texts.clear();
	unsigned short offsetX = TILE_SIZE * OFFSET_X;
	unsigned short offsetY = TILE_SIZE * (2 * OFFSET_Y + game.level.height);
	unsigned short lineWidth = SCREEN_WIDTH / TILE_SIZE - OFFSET_X;
	for (unsigned short level = 1; level <= game.levelSet.levels; ++level) {
		sf::Text text;
		sf::Color realColor = getTextColor(level);
		text.setFont(game.graphics.font);
		text.setString(std::to_string(level));
		text.setFillColor(realColor);
		text.setOrigin(0, TILE_SIZE / 2);
		text.setCharacterSize(TEXT_SIZE);
		text.setPosition(offsetX + TILE_SIZE * ((level - 1) % lineWidth), offsetY + TILE_SIZE * ((level - 1) / lineWidth));
		texts.push_back(text);
	}
}