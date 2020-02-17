#ifndef LEVELSET_HPP
#define LEVELSET_HPP

#include "constants.hpp"

class Game;

struct LevelSet {
private:
	unsigned short currentLevel;
	unsigned short levels;
	std::vector<LevelState> levelStates;
	std::string name;
public:
	Game* game;

	bool checkSetSave(const std::string &levelSetName);
	void loadSet(const std::string &levelSetName);
	void loadSet();
	void saveCurrentLevel(const std::string &levelSetName);
	void saveCurrentLevel();
	bool checkSet(const std::string &levelSetName);
	void saveSet(const std::string &levelSetName, bool save = true);
	void saveSet(bool save = true);

	std::string getName();
	void setName(std::string newName);

	unsigned short getCurrentLevel();
	unsigned short getLevelsCount();
	void setCurrentLevel(short level);
	void nextLevel();
	void previousLevel();

	bool isLevelFirst();
	bool isLevelLast();
	bool isLevelInRange(short level);
	void passLevel();
	void unlockNextLevel();

	void addLevel();
	LevelState getLevelState();
	LevelState getLevelState(short level);
};

#endif
