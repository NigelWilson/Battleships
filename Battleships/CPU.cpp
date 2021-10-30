#include "CPU.h"
#include <iostream>
#include <random>
#include <sstream>

using u32 = uint_least32_t;
using engine = std::mt19937;

CPU::CPU(){}

CPU::~CPU(){}

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

// Needs more work - may have to forego the intelligence side initially to get it up and going
std::vector<int> CPU::attack()
{
    std::vector<int> attackCoordinates;

    if (hits.size() > 0)
    {
        std::vector<std::string> split;
        std::stringstream test(hits.back());
        std::string segment;

        while (std::getline(test, segment, ':'))
        {
            split.push_back(segment);
        }

        int front = std::stoi(split.front());
        int back = std::stoi(split.back());

        if (front + 1 == 8)
            direction++;
        else if (front - 1 == 0)
            direction++;
        else if (back + 1 == 8)
            direction++;
        else if (back - 1 == 0)
            direction = 0;

        // Doesn't work overly well but OK for now
        switch (direction)
        {
        case 0:
            if (std::stoi(split.front()) == 8) break; // Does this allow fall through to case 1?
            attackCoordinates.push_back(std::stoi(split.front()) + 1);
            attackCoordinates.push_back(std::stoi(split.back()));
            direction++;
            break;
        case 1:
            if (std::stoi(split.front()) == 1) break;
            attackCoordinates.push_back(std::stoi(split.front()) - 1);
            attackCoordinates.push_back(std::stoi(split.back()));
            direction++;
            break;
        case 2:
            if (std::stoi(split.back()) == 8) break;
            attackCoordinates.push_back(std::stoi(split.front()));
            attackCoordinates.push_back(std::stoi(split.back()) + 1);
            direction++;
            break;
        case 3:
            if (std::stoi(split.back()) == 1)
            {
                hits.pop();
                direction = 0;
                break;
            }
            attackCoordinates.push_back(std::stoi(split.front()));
            attackCoordinates.push_back(std::stoi(split.back()) - 1);
            hits.pop();
            direction = 0;
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

void CPU::addHit(int row, int col)
{
    direction = 0;
    hits.push(std::to_string(row) + ":" + std::to_string(col));
    if (hits.size() > 3)
    {
        hits.pop();
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
