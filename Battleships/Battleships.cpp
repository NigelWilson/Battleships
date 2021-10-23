// Battleships.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <random>
#include <string>
#include <Windows.h>
#include <nlohmann/json.hpp>
#include <queue>
#include <vector>
#include <sstream>
#include "Ship.h"

using u32 = uint_least32_t;
using engine = std::mt19937;

void draw();
void drawPlayerGrid();
void drawCPUGrid();
void addShipsCPU(std::vector<Ship*> const &ships);
void addShipsPlayer(std::vector<Ship*> const &playerShips);
int rng(int r1, int r2);
void attack();
void playerAttack();
bool validShipPosition(int coordinate, int direction, int spaces);
bool validCoordinates(int c1a, int c1b, int c2a, int c2b);
bool checkWinStatus(std::string player);

//enum ships {CARRIER, BATTLESHIP, DESTROYER, SUBMARINE, CRUISER, END};
//enum ship;
nlohmann::json data = "{\"ships\":{\"type\":\"carrier\",\"spaces\":5,\"sprite\":\"C\"}}"_json;
bool hit = false;
std::queue<std::string> hits;
int direction = 0;
bool gameOver = false;
bool debug = true;
std::string winner = "";

char grid[9][9] = {
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

char gridCPU[9][9] = {
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

char gridCPUDisplay[9][9] = {
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

const std::map<char, int> alphaChars{ {'A', 1}, {'B', 2}, {'C', 3}, {'D', 4}, {'E', 5}, {'F', 6}, {'G', 7}, {'H', 8} };
std::vector<Ship*> ships;
std::vector<Ship*> playerShips;

int main()
{
    // Pass this into addShips function and for each arg place ship on grid and perhaps set coordinates onto the ship object? 
    Ship* cpuCarrier = new Ship("carrier", 'C', 5);
    Ship* cpuBattleship = new Ship("battleship", 'B', 4);
    Ship* cpuDestroyer = new Ship("destroyer", 'D', 3);
    Ship* cpuSubmarine = new Ship("submarine", 'S', 3);
    Ship* cpuPatrolBoat = new Ship("patrol boat", 'P', 2);
    ships.push_back(cpuCarrier);
    ships.push_back(cpuBattleship);
    ships.push_back(cpuDestroyer);
    ships.push_back(cpuSubmarine);
    ships.push_back(cpuPatrolBoat);
    std::cout << &ships << std::endl;

    Ship* playerCarrier = new Ship("carrier", 'C', 5);
    Ship* playerBattleship = new Ship("battleship", 'B', 4);
    Ship* playerDestroyer = new Ship("destroyer", 'D', 3);
    Ship* playerSubmarine = new Ship("submarine", 'S', 3);
    Ship* playerPatrolBoat = new Ship("patrol boat", 'P', 2);
    playerShips.push_back(playerCarrier);
    playerShips.push_back(playerBattleship);
    playerShips.push_back(playerDestroyer);
    playerShips.push_back(playerSubmarine);
    playerShips.push_back(playerPatrolBoat);

    addShipsCPU(ships);
    addShipsPlayer(playerShips);

    while (!gameOver)
    {
        draw();
        playerAttack();
        attack();
        Sleep(1000);
    }

    std::cout << winner << " wins!" << std::endl;
}

void draw()
{
    system("cls");
    std::cout << "Player Grid" << std::endl;
    drawPlayerGrid();

    std::cout << std::endl;
    std::cout << "CPU Grid" << std::endl;
    drawCPUGrid();
}

void drawPlayerGrid()
{
    for (int i = 0; i < sizeof(grid[0]); i++)
    {
        for (int j = 0; j < sizeof(grid[0]); j++)
        {
            std::cout << grid[i][j];
            if (j == 8) std::cout << std::endl;
        }
    }
}

void drawCPUGrid()
{
    for (int i = 0; i < sizeof(gridCPUDisplay[0]); i++)
    {
        for (int j = 0; j < sizeof(gridCPUDisplay[0]); j++)
        {
            std::cout << gridCPUDisplay[i][j];
            if (j == 8) std::cout << std::endl;
        }
    }
}

// Constant reference to vector of pointers
// Without const, compiler believes we will change the pointers
void addShipsCPU(std::vector<Ship*> const &ships)
{
    /* Refactored to use objects instead of JSON
    int spaces = data["ships"]["spaces"];
    std::string ship = data["ships"]["type"];*/

    for (Ship* ship : ships)
    {
        int direction = rng(1, 2);

        int coordinate = rng(1, 8);

        while (!validShipPosition(coordinate, direction, ship->getSpaces()))
        {
            coordinate = rng(1, 8);
        }

        for (int i = 0; i < ship->getSpaces(); i++)
        {
            switch (direction)
            {
            case 1:
                gridCPU[coordinate][coordinate + i] = ship->getSprite();
                break;
            case 2:
                gridCPU[coordinate + i][coordinate] = ship->getSprite();
                break;
            }
        }
    }
}

bool validShipPosition(int coordinate, int direction, int spaces)
{
    if (coordinate + spaces > 8)
        return false;
    
    for (int i = 0; i < spaces; i++)
    {
        switch (direction)
        {
        case 1:
            if (gridCPU[coordinate][coordinate + i] != ' ')
            {
                return false;
            }
            break;
        case 2:
            if (gridCPU[coordinate + i][coordinate] != ' ')
            {
                return false;
            }
            break;
        }
    }
    return true;
}

void addShipsPlayer(std::vector<Ship*> const &playerShips)
{
    std::cout << "Welcome to Battleships!" << std::endl;

    for (Ship* ship : playerShips) 
    {
        drawPlayerGrid();
        int c1a = 0, c1b = 0, c2a = 0, c2b = 0;

        do
        {
            std::cout << "Please enter coordianates for your " << ship->getType() << " separated by a colon e.g.A1:A5" << std::endl;
            std::cout << "You need " << ship->getSpaces() << " spaces to place your " << ship->getType() << std::endl;
            std::string input;
            std::cin >> input;

            std::cout << "You entered: " << input << std::endl;

            std::vector<std::string> split;
            std::stringstream test(input);
            std::string segment;

            while (std::getline(test, segment, ':'))
            {
                split.push_back(segment);
            }

            std::string front = split.front();
            std::string back = split.back();

            try
            {
                // TODO Need to sanitize the inputs more, but assume correct format for now
                c1a = alphaChars.at(front[0]);
                c1b = char(front[1]) - '0'; // returns ascii value, interesting! Subtract char '0' to get the actual char value as int
                c2a = alphaChars.at(back[0]);
                c2b = char(back[1]) - '0';
            }
            catch (std::out_of_range)
            {
                std::cout << "You gave rubbish inputs, game over!" << std::endl;
                gameOver = true;
            }
        } while (!validCoordinates(c1a, c1b, c2a, c2b));

        // One of the axis must have the same value, check which one and then fill in the blanks on the other
        if (c1b == c2b)
        {
            for (int i = c1a; i <= c2a; i++)
            {
                grid[c1b][i] = ship->getSprite();
            }
        }
        else
        {
            for (int i = c1b; i <= c2b; i++)
            {
                grid[i][c2a] = ship->getSprite();
            }
        }
    }
}

// Player input ship positions validity checker
bool validCoordinates(int c1a, int c1b, int c2a, int c2b)
{
    if (c1a < 1 || c2a > 8 || c2a < 1 || c2b > 8)
    {
        return false;
    }

    if (c1b == c2b)
    {
        for (int i = c1a; i <= c2a; i++)
        {
            if (grid[c1b][i] != ' ')
            {
                return false;
            }
        }
    }
    else
    {
        for (int i = c1b; i <= c2b; i++)
        {
            if (grid[i][c2a] != ' ')
            {
                return false;
            }
        }
    }

    return true;
}

// Trying to attack around a previous hit isn't working too well so can improve that
// CPU winning the game does work, but will not display the final X due to code order
void attack() 
{
    int coordinates[2] = { 0, 0 };

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

        if (front + 1 == 8 || grid[front+1][back] != ' ')
            direction++;
        else if (front - 1 == 0 || grid[front - 1][back] != ' ')
            direction++;
        else if (back + 1 == 8 || grid[front][back + 1] != ' ')
            direction++;
        else if (back - 1 == 0 || grid[front][back - 1] != ' ')
            direction = 0;

        // Doesn't work overly well but OK for now
        switch (direction)
        {
        case 0:
            if (std::stoi(split.front()) == 8) break;
            coordinates[0] = std::stoi(split.front())+1;
            coordinates[1] = std::stoi(split.back());
            direction++;
            break;
        case 1:
            if (std::stoi(split.front()) == 1) break;
            coordinates[0] = std::stoi(split.front())-1;
            coordinates[1] = std::stoi(split.back());
            direction++;
            break;
        case 2:
            if (std::stoi(split.back()) == 8) break;
            coordinates[0] = std::stoi(split.front());
            coordinates[1] = std::stoi(split.back())+1;
            direction++;
            break;
        case 3:
            if (std::stoi(split.back()) == 1)
            {
                hits.pop();
                direction = 0;
                break;
            }
            coordinates[0] = std::stoi(split.front());
            coordinates[1] = std::stoi(split.back())-1;
            hits.pop();
            direction = 0;
            break;
        }
    }

    
    if (grid[coordinates[0]][coordinates[1]] == 'X' || grid[coordinates[0]][coordinates[1]] == 'O')
    {
        coordinates[0] = 0;
        coordinates[1] = 0;
    }

    if (coordinates[0] == 0 && coordinates[1] == 0)
    {
        int x = 0;
        int y = 0;

        do
        {
            x = rng(1, 8);
            y = rng(1, 8);
        } 
        while ((grid[y][x] == 'X') || (grid[y][x] == 'O'));

        coordinates[0] = y;
        coordinates[1] = x;
    }
    
    if (grid[coordinates[0]][coordinates[1]] != ' ' && grid[coordinates[0]][coordinates[1]] != 'O' && grid[coordinates[0]][coordinates[1]] != 'X')
    {
        char spaceHit = grid[coordinates[0]][coordinates[1]];
        grid[coordinates[0]][coordinates[1]] = 'X';

        // ships is available globally but we passed it by reference because we can earlier...refactor later perhaps
        for (Ship* ship : ships) 
        {
            if (spaceHit == ship->getSprite())
            {
                ship->setHits(ship->getHits()+1);
                if (ship->getSpaces() == ship->getHits())
                {
                    ship->setDestroyed(true);
                    std::cout << "players " << ship->getType() << " was destroyed!" << std::endl;
                    gameOver = checkWinStatus("CPU");
                }
            }
        }
        
        hit = true;
        direction = 0;
        hits.push(std::to_string(coordinates[0]) + ":" + std::to_string(coordinates[1]));
        if (hits.size() > 3) 
        {
            hits.pop();
        }
        //std::cout << hits.front() << std::endl;
    }
    else
    {
        grid[coordinates[0]][coordinates[1]] = 'O';
        hit = false;
    }
    std::cout << coordinates[0] << ':' << coordinates[1] << std::endl;
}

void playerAttack()
{
    int c1a = 0, c1b = 0;

    do
    {
        std::cout << "Input coordinates to attack: " << std::endl;
        std::string input;
        std::cin >> input;

        try
        {
            // TODO Need to sanitize the inputs more, but assume correct format for now
            c1b = alphaChars.at(input[0]);
            c1a = char(input[1]) - '0'; // returns ascii value, interesting! Subtract char '0' to get the actual char value as int
        }
        catch (std::out_of_range)
        {
            std::cout << "You gave rubbish inputs, game over!" << std::endl;
            gameOver = true;
        }
    } while (gridCPU[c1a][c1b] == 'O' || gridCPU[c1a][c1b] == 'X');

    if (gridCPU[c1a][c1b] != ' ' && gridCPU[c1a][c1b] != 'O' && gridCPU[c1a][c1b] != 'X')
    {
        char spaceHit = gridCPU[c1a][c1b];
        gridCPU[c1a][c1b] = 'X';
        gridCPUDisplay[c1a][c1b] = 'X';

        // WRONG NAME! REFACTOR TO CPUSHIPS
        for (Ship* ship : playerShips)
        {
            if (spaceHit == ship->getSprite())
            {
                ship->setHits(ship->getHits() + 1);
                if (ship->getSpaces() == ship->getHits())
                {
                    ship->setDestroyed(true);
                    std::cout << "CPU " << ship->getType() << " was destroyed!" << std::endl;
                    gameOver = checkWinStatus("Player");
                }
            }
        }
    }
    else
    {
        gridCPU[c1a][c1b] = 'O';
        gridCPUDisplay[c1a][c1b] = 'O';
    }
}

bool checkWinStatus(std::string player)
{
    if (player == "CPU")
    {
        for (Ship* ship : ships)
        {
            if (!ship->isDestroyed())
            {
                return false;
            }
        }
        winner = "CPU";
    }
    else
    {
        for (Ship* ship : playerShips)
        {
            if (!ship->isDestroyed())
            {
                return false;
            }
        }
        winner = "Player";
    }
    return true;
}

int rng(int r1, int r2)
{
    std::random_device os_seed;
    const u32 seed = os_seed();

    engine generator(seed);
    std::uniform_int_distribution<u32> dist(r1, r2);

    return dist(generator);
}