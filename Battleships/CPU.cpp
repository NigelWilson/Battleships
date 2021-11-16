#include "CPU.h"
#include <iostream>
#include <random>
#include <sstream>

using u32 = uint_least32_t;
using engine = std::mt19937;

CPU::CPU(){}

CPU::~CPU(){}

// deprecated but left for debug
void CPU::draw()
{
    std::cout << "CPU grid" << std::endl;
    Player::draw();
}

void CPU::addShips()
{
    for (Ship* ship : this->getShips())
    {
        int direction = rng(1, 2);

        int coordinate = rng(1, 8);

        while (!validShipCoordinates(coordinate, direction, ship->getSpaces()))
        {
            coordinate = rng(1, 8);
        }

        for (int i = 0; i < ship->getSpaces(); i++)
        {
            switch (direction)
            {
            case 1:
                this->modifyGrid(coordinate, coordinate + i, ship->getSprite());
                break;
            case 2:
                this->modifyGrid(coordinate + i, coordinate, ship->getSprite());
                break;
            }
        }
    }
}

bool CPU::validShipCoordinates(int coordinate, int direction, int spaces)
{
    if (coordinate + spaces > 8)
        return false;

    for (int i = 0; i < spaces; i++)
    {
        switch (direction)
        {
        case 1:
            if (this->getGrid()[coordinate][coordinate + i] != ' ')
            {
                return false;
            }
            break;
        case 2:
            if (this->getGrid()[coordinate + i][coordinate] != ' ')
            {
                return false;
            }
            break;
        }
    }
    return true;
}

// Based on hits we remember from previous turns, attack surrounding spaces
std::vector<int> CPU::attack()
{
    std::vector<int> attackCoordinates;

    for (int i = 0; i < hits.size(); i++)
    {
        std::vector<std::string> split;
        std::stringstream stream(hits.back());
        std::string segment;

        while (std::getline(stream, segment, ':'))
        {
            split.push_back(segment);
        }

        switch (direction)
        {
        case 0:
            if (std::stoi(split.front()) != 8)
            {
                attackCoordinates.push_back(std::stoi(split.front()) + 1);
                attackCoordinates.push_back(std::stoi(split.back()));
                direction++;
                break;
            }
        case 1:
            if (std::stoi(split.front()) != 1)
            {
                attackCoordinates.push_back(std::stoi(split.front()) - 1);
                attackCoordinates.push_back(std::stoi(split.back()));
                direction++;
                break;
            }
        case 2:
            if (std::stoi(split.back()) != 8)
            {
                attackCoordinates.push_back(std::stoi(split.front()));
                attackCoordinates.push_back(std::stoi(split.back()) + 1);
                direction++;
                break;
            }
        case 3:
            if (std::stoi(split.back()) == 1)
            {
                hits.pop_back();
                direction = 0;
                break;
            }
            attackCoordinates.push_back(std::stoi(split.front()));
            attackCoordinates.push_back(std::stoi(split.back()) - 1);
            hits.pop_back();
            direction = 0;
            break;
        }

        if (attackCoordinates.size())
        {
            break;
        }
    }

    if (!attackCoordinates.size())
    {
        attackCoordinates.push_back(rng(1, 8));
        attackCoordinates.push_back(rng(1, 8));
    }
    
    return attackCoordinates;
}

// Remember the last 3 hits, could change CPU difficulty by remembering more or less hits
void CPU::addHit(int row, int col)
{
    direction = 0;
    hits.push_back(std::to_string(row) + ":" + std::to_string(col));
    if (hits.size() > 3)
    {
        hits.pop_front();
    }
}

int CPU::rng(int r1, int r2)
{
    std::random_device os_seed;
    const u32 seed = os_seed();

    engine generator(seed);
    std::uniform_int_distribution<u32> dist(r1, r2);

    return dist(generator);
}
