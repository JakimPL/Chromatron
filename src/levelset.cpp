#include <fstream>

#include "game.h"
#include "auxiliary.h"
#include "log.h"

void Game::LevelSet::loadSet(const std::string &levelSetName)
{
	std::string location = PATH_DATA + PATH_LEV_PREFIX + levelSetName + PATH_LS_SUFFIX + (checkSetSave(levelSetName) ? PATH_SAV_SUFFIX : PATH_SET_SUFFIX);
	std::ifstream levelSetFile(location);
	if (levelSetFile.good()) {
		name = levelSetName;
		readByte(levelSetFile, levels);
		readByte(levelSetFile, currentLevel);

		for (size_t level = 0; level < levels; ++level) {
			unsigned short levelState;
			readByte(levelSetFile, levelState);
			levelStates.push_back(static_cast<LevelState>(levelState));
		}

		levelSetFile.close();
		LogInfo("File " + location + " loaded successfully");
	} else {
		LogError("Failed to save " + location + " file");
		throw std::runtime_error("failed to load " + location + " file");
	}

	game->levelId = numberToString(currentLevel);
	game->level.loadLevel(currentLevel);
}

bool Game::LevelSet::checkSet(const std::string &levelSetName)
{
	std::string location = PATH_DATA + PATH_LEV_PREFIX + levelSetName + PATH_LS_SUFFIX + (checkSetSave(levelSetName) ? PATH_SAV_SUFFIX : PATH_SET_SUFFIX);
	std::ifstream levelSetFile(location);
	return (levelSetFile.good());
}

bool Game::LevelSet::checkSetSave(const std::string &levelSetName)
{
	std::string location = PATH_DATA + PATH_LEV_PREFIX + levelSetName + PATH_LS_SUFFIX + PATH_SAV_SUFFIX;
	std::ifstream levelSetSaveFile(location);
	return levelSetSaveFile.good();
}

void Game::LevelSet::saveSet(const std::string &levelSetName)
{
	std::string location = PATH_DATA + PATH_LEV_PREFIX + levelSetName + PATH_LS_SUFFIX + PATH_SAV_SUFFIX;
	std::ofstream levelSetFile(location, std::ios::binary);
	if (levelSetFile.good()) {
		writeByte(levelSetFile, levels);
		writeByte(levelSetFile, currentLevel);

		for (size_t level = 0; level < levels; ++level) {
			writeByte(levelSetFile, levelStates[level]);
		}

		levelSetFile.close();
		LogInfo("File " + location + " saved successfully");
	} else {
		LogError("Failed to save " + location + " file");
		throw std::runtime_error("failed to save " + location + " file");
	}

	saveCurrentLevel();
}

void Game::LevelSet::saveCurrentLevel()
{
	saveCurrentLevel(name);
}

void Game::LevelSet::saveCurrentLevel(const std::string &levelSetName)
{
	std::string location = PATH_DATA + PATH_LEV_PREFIX + levelSetName + "/" + numberToString(currentLevel) + PATH_SAV_SUFFIX;
	std::ofstream levelSetFile(location, std::ios::binary);
	if (levelSetFile.good()) {
		unsigned short stackObjectsCount = game->level.stackObjectList.size();
		writeByte(levelSetFile, stackObjectsCount);

		for (size_t index = 0; index < stackObjectsCount; ++index) {
			writeObject(levelSetFile, game->level.stackObjectList[index]);
		}

		levelSetFile.close();
		LogInfo("File " + location + " saved successfully");
	} else {
		LogError("Failed to save " + location + " file");
		throw std::runtime_error("failed to save " + location + " file");
	}
}

bool Game::LevelSet::isLevelFirst()
{
	return currentLevel == 1;
}

bool Game::LevelSet::isLevelLast()
{
	return currentLevel == levels;
}

void Game::LevelSet::unlockNextLevel()
{
	for (size_t level = 0; level < levels; ++level) {
		if (levelStates[level] == LS_LOCKED) {
			levelStates[level] = LS_AVAILABLE;
			break;
		}
	}
}

void Game::LevelSet::saveSet()
{
	saveSet(name);
}