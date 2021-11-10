#pragma once
#include <vector>
#include <array>
#include "Ship.h"
class Player
{
private:
	std::array<std::array<char, 9>, 9> grid;
	std::array<std::array<char, 9>, 9> displayGrid;
	std::vector<Ship*> ships;
public:
	Player();
	~Player();

	void createShips();
	void buildGrid();
	void buildDisplayGrid();
	void modifyGrid(int row, int column, char value);
	void modifyDisplayGrid(int row, int column, char value);
	virtual void draw();
	void drawDisplayGrid();

	std::array<std::array<char, 9>, 9> & getGrid();
	std::array<std::array<char, 9>, 9>& getDisplayGrid();
	std::vector<Ship*> getShips();
	char applyImpact(std::vector<int> attackCoordinates, bool isMultiplayerGame, bool isAttacker);

	virtual std::vector<int> attack();
};

