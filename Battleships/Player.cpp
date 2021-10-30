#include "Player.h"
#include <iostream>

Player::Player()
{
    createShips();
    buildGrid();
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

// Note the & to return a reference
std::array<std::array<char, 9>, 9> & Player::getGrid()
{
    //std::cout << "Player.cpp: " << &this->getGrid() << std::endl;
    return this->grid;
    
}

void Player::modifyGrid(int row, int column, char value)
{
    this->grid[row][column] = value;
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

char Player::applyImpact(std::vector<int> attackCoordinates)
{
    char pos = this->getGrid()[attackCoordinates[0]][attackCoordinates[1]];
    // should we return this value and have the main class track what's been destroyed or just update it here?

    if (pos == ' ')
    {
        this->modifyGrid(attackCoordinates[0], attackCoordinates[1], 'O');
    }
    else
    {
        this->modifyGrid(attackCoordinates[0], attackCoordinates[1], 'X');
    }

    return pos;
}