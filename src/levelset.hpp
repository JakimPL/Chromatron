#ifndef LEVELSET_HPP
#define LEVELSET_HPP

#include "constants.hpp"

class Game;

///TODO: name, levels, levelStates private

struct LevelSet {
private:
	unsigned short currentLevel;
	unsigned short levels;
	std::vector<LevelState> levelStates;
public:
	std::string name;
	Game* game;

	bool checkSetSave(const std::string &levelSetName);
	void loadSet(const std::string &levelSetName);
	void saveCurrentLevel(const std::string &levelSetName);
	bool checkSet(const std::string &levelSetName);
	void saveSet(const std::string &levelSetName, bool save = true);
	void saveCurrentLevel();
	void saveSet(bool save = true);

	unsigned short getCurrentLevel();
	unsigned short getLevelsCount();
	void setCurrentLevel(short level);
	void nextLevel();
	void previousLevel();

	bool isLevelFirst();
	bool isLevelLast();
	void passLevel();
	void unlockNextLevel();

	void addLevel();
	LevelState getLevelState();
	LevelState getLevelState(short level);
};

#endif
