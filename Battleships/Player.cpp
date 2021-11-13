#include "Player.h"
#include <iostream>

Player::Player()
{
    createShips();
    buildGrid();
    buildDisplayGrid();
}

Player::~Player()
{
    for (Ship* ship : this->getShips())
    {
        delete ship;
    }
}

void Player::buildGrid()
{
    this->grid = {
        ' ', 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H',
        '1', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',
        '2', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',
        '3', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',
        '4', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',
        '5', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',
        '6', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',
        '7', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',
        '8', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',
    };
}

void Player::buildDisplayGrid()
{
    this->displayGrid = {
        ' ', 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H',
        '1', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',
        '2', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',
        '3', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',
        '4', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',
        '5', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',
        '6', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',
        '7', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',
        '8', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',
    };
}

void Player::createShips()
{
    Ship* carrier = new Ship("carrier", 'C', 5);
    Ship* battleship = new Ship("battleship", 'B', 4);
    Ship* destroyer = new Ship("destroyer", 'D', 3);
    Ship* submarine = new Ship("submarine", 'S', 3);
    Ship* patrolBoat = new Ship("patrol boat", 'P', 2);
    this->ships.push_back(carrier);
    this->ships.push_back(battleship);
    this->ships.push_back(destroyer);
    this->ships.push_back(submarine);
    this->ships.push_back(patrolBoat);
}

void Player::draw()
{
    for (int i = 0; i < sizeof(this->grid[0]); i++)
    {
        for (int j = 0; j < sizeof(this->grid[0]); j++)
        {
            std::cout << this->grid[i][j];
            if (j == 8) std::cout << std::endl;
        }
    }
}

void Player::drawDisplayGrid()
{
    for (int i = 0; i < sizeof(this->displayGrid[0]); i++)
    {
        for (int j = 0; j < sizeof(this->displayGrid[0]); j++)
        {
            std::cout << this->displayGrid[i][j];
            if (j == 8) std::cout << std::endl;
        }
    }
}

std::array<std::array<char, 9>, 9> & Player::getGrid()
{
    return this->grid;
}

std::array<std::array<char, 9>, 9>& Player::getDisplayGrid()
{
    return this->displayGrid;
}

void Player::modifyGrid(int row, int column, char value)
{
    this->grid[row][column] = value;
}

void Player::modifyDisplayGrid(int row, int column, char value)
{
    this->displayGrid[row][column] = value;
}

// Can we (should we) return this as a reference?
std::vector<Ship*> Player::getShips()
{
	return this->ships;
}

std::vector<int> Player::attack()
{
    return std::vector<int>();
}

char Player::applyImpact(std::vector<int> attackCoordinates, bool isMultiplayerGame, bool isAttacker, char pos)
{
    if (!pos)
    {
        pos = this->getGrid()[attackCoordinates[0]][attackCoordinates[1]];
    }

    if (isMultiplayerGame) 
    {
        if (pos == ' ')
        {
            if (!isAttacker)
            {
                this->modifyGrid(attackCoordinates[0], attackCoordinates[1], 'O');
            }
            else
            {
                this->modifyDisplayGrid(attackCoordinates[0], attackCoordinates[1], 'O');
            }
        }
        else
        {
            if (!isAttacker)
            {
                this->modifyGrid(attackCoordinates[0], attackCoordinates[1], 'X');
            }
            else
            {
                this->modifyDisplayGrid(attackCoordinates[0], attackCoordinates[1], 'X');
            }
        }
    }
    else
    {
        if (pos == ' ')
        {
            this->modifyGrid(attackCoordinates[0], attackCoordinates[1], 'O');
            this->modifyDisplayGrid(attackCoordinates[0], attackCoordinates[1], 'O');
        }
        else
        {
            this->modifyGrid(attackCoordinates[0], attackCoordinates[1], 'X');
            this->modifyDisplayGrid(attackCoordinates[0], attackCoordinates[1], 'X');
        }
    }

    return pos;
}
