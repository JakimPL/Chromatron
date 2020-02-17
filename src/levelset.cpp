#include <fstream>

#include "game.hpp"
#include "auxiliary.hpp"
#include "log.hpp"

void LevelSet::loadSet(const std::string &levelSetName)
{
	std::string location = PATH_DATA + PATH_LEV_PREFIX + levelSetName + PATH_LS_SUFFIX + PATH_SET_SUFFIX;
	std::ifstream levelSetFile(location);
	if (levelSetFile.good()) {
		name = levelSetName;
		readByte(levelSetFile, levels);

		if (checkSetSave(levelSetName)) {
			levelSetFile.close();
			LogInfo("File " + location + " loaded successfully");
			location = PATH_DATA + PATH_LEV_PREFIX + levelSetName + PATH_LS_SUFFIX + PATH_SAV_SUFFIX;
			levelSetFile.open(location, std::ios::binary | std::ios::in);

			readByte(levelSetFile, currentLevel);

			for (size_t level = 0; level < levels; ++level) {
				unsigned short levelState;
				readByte(levelSetFile, levelState);
				levelStates.push_back(static_cast<LevelState>(levelState));
			}
		} else {
			for (size_t level = 0; level < levels; ++level) {
				unsigned short levelState;
				readByte(levelSetFile, levelState);
				levelStates.push_back(static_cast<LevelState>(LS_LOCKED));
			}

			if (levels > 0) {
				currentLevel = 1;
				levelStates[0] = LS_AVAILABLE;
			} else {
				LogError("No levels in a levelset");
			}
		}

		levelSetFile.close();
		LogInfo("File " + location + " loaded successfully");
	} else {
		LogError("Failed to save " + location + " file");
	}

	game->levelId = numberToString(currentLevel);
	game->level.loadLevel(currentLevel);
}

void LevelSet::loadSet()
{
	loadSet(name);
}

bool LevelSet::checkSet(const std::string &levelSetName)
{
	std::string location = PATH_DATA + PATH_LEV_PREFIX + levelSetName + PATH_LS_SUFFIX + (checkSetSave(levelSetName) ? PATH_SAV_SUFFIX : PATH_SET_SUFFIX);
	std::ifstream levelSetFile(location);
	return (levelSetFile.good());
}

bool LevelSet::checkSetSave(const std::string &levelSetName)
{
	std::string location = PATH_DATA + PATH_LEV_PREFIX + levelSetName + PATH_LS_SUFFIX + PATH_SAV_SUFFIX;
	std::ifstream levelSetSaveFile(location);
	return levelSetSaveFile.good();
}

void LevelSet::saveSet(bool save)
{
	saveSet(name, save);
}

void LevelSet::saveSet(const std::string &levelSetName, bool save)
{
	std::string location = PATH_DATA + PATH_LEV_PREFIX + levelSetName + PATH_LS_SUFFIX + (save ? PATH_SAV_SUFFIX : PATH_SET_SUFFIX);
	std::ofstream levelSetFile(location, std::ios::binary);
	if (levelSetFile.good()) {
		if (!save) {
			writeByte(levelSetFile, levels);
		} else {
			writeByte(levelSetFile, currentLevel);

			for (size_t level = 0; level < levels; ++level) {
				writeByte(levelSetFile, levelStates[level]);
			}
		}

		levelSetFile.close();
		LogInfo("File " + location + " saved successfully");
	} else {
		LogError("Failed to save " + location + " file");
	}

	saveCurrentLevel();
}

void LevelSet::saveCurrentLevel()
{
	saveCurrentLevel(name);
}

void LevelSet::saveCurrentLevel(const std::string &levelSetName)
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
	}
}

std::string LevelSet::getName()
{
	return name;
}

void LevelSet::setName(std::string newName)
{
	name = newName;
}

bool LevelSet::isLevelFirst()
{
	return currentLevel == 1;
}

bool LevelSet::isLevelLast()
{
	return currentLevel == levels;
}

void LevelSet::passLevel()
{
	if (getLevelState() == LS_AVAILABLE) {
		levelStates[currentLevel - 1] = LS_PASSED;
		unlockNextLevel();
	}
}

void LevelSet::unlockNextLevel()
{
	for (size_t level = 0; level < levels; ++level) {
		if (levelStates[level] == LS_LOCKED) {
			levelStates[level] = LS_AVAILABLE;
			break;
		}
	}
}

unsigned short LevelSet::getLevelsCount()
{
	return levels;
}

unsigned short LevelSet::getCurrentLevel()
{
	return currentLevel;
}

void LevelSet::setCurrentLevel(short level)
{
	if (0 < level and level <= levels) {
		if (getLevelState(level) != LS_LOCKED) {
			currentLevel = level;
		}

		game->levelId = numberToString(currentLevel);
	}
}

void LevelSet::nextLevel()
{
	setCurrentLevel(currentLevel + 1);
}

void LevelSet::previousLevel()
{
	setCurrentLevel(currentLevel - 1);
}

void LevelSet::addLevel()
{
	levels++;
}

LevelState LevelSet::getLevelState()
{
	///TODO: 0 < level <= levels
	return getLevelState(currentLevel);
}

LevelState LevelSet::getLevelState(short level)
{
	return levelStates[level - 1];
}
