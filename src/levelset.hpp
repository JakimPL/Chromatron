#ifndef LEVELSET_HPP
#define LEVELSET_HPP

#include "constants.hpp"

class Game;

struct LevelSet {
private:
	unsigned short currentLevel;
public:
	std::string name;
	unsigned short levels;
	std::vector<LevelState> levelStates;
	Game* game;

	bool checkSetSave(const std::string &levelSetName);
	void loadSet(const std::string &levelSetName);
	void saveCurrentLevel(const std::string &levelSetName);
	bool checkSet(const std::string &levelSetName);
	void saveSet(const std::string &levelSetName, bool save = true);
	void saveCurrentLevel();
	void saveSet(bool save = true);

	unsigned short getCurrentLevel();
	void nextLevel();
	void previousLevel();

	bool isLevelFirst();
	bool isLevelLast();
	void unlockNextLevel();
};

#endif