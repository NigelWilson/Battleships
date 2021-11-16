#pragma once
#include "Player.h"
#include <map>
class Human :
    public Player
{
private:
    // initialising here will force recompile for any classes importing this header if the values change
    const std::map<char, int> alphaChars = { {'A', 1}, {'B', 2}, {'C', 3}, {'D', 4}, {'E', 5}, {'F', 6}, {'G', 7}, {'H', 8} };
    std::string inputError = "";
public:
    Human();
    ~Human();

    void draw();
    void addShips();
    bool validShipCoordinates(int c1a, int c1b, int c2a, int c2b, Ship* ship);
    std::vector<int> attack();
};

