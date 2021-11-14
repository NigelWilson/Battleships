#pragma once
#include "Player.h"
#include <deque>
class CPU :
    public Player
{
private:
    bool hit = false;
    int direction = 0;
    std::deque<std::string> hits;
public:
    CPU();
    ~CPU();

    void draw();
    void addShips();
    bool validShipCoordinates(int coordinate, int direction, int spaces);
    std::vector<int> attack();
    int rng(int r1, int r2);
    void addHit(int row, int col);
};

