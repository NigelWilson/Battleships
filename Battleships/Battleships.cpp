// Battleships.cpp : This file contains the 'main' function. Program execution begins and ends there.
#include <iostream>
#include <string>
#include <Windows.h>
#include <vector>
#include <typeinfo>
#include "Ship.h"
#include "Human.h"
#include "CPU.h"

void draw(Player* playerOne, Player* playerTwo);
bool isAttackCoordinatesValid(std::vector<int> attackCoordinates, Player* player);
std::string updateGameState(Player* player, char hitPos);
bool checkWinStatus(Player* player);
void intro();
void playSinglePlayer();

bool gameOver = false;
std::string winner = "";
std::vector<int> attackCoordinates;

int main()
{
    intro();
}

void intro()
{
    std::cout << "Welcome to Battleships!" << std::endl;
    std::cout << std::endl;
    std::cout << "Please choose a game type:" << std::endl;
    std::cout << "1. Single Player" << std::endl;
    std::cout << "2. Multiplayer" << std::endl;
    std::cout << "3. Quit" << std::endl;

    std::string input;
    std::cin >> input;

    if (std::stoi(input) == 1)
    {
        system("cls");
        playSinglePlayer();
    }
    else if (std::stoi(input) == 2)
    {
        //playMultiplayer()
        std::cout << "Coming soon!" << std::endl;
        Sleep(5000);
        system("cls");
        intro();
    }
    else
    {
        return;
    }
}

void playSinglePlayer()
{
    Human* human = new Human();
    CPU* cpu = new CPU();
    human->addShips();
    cpu->addShips();
    draw(human, cpu);

    while (!gameOver)
    {
        attackCoordinates.clear();
        do
        {
            attackCoordinates = human->attack();
        } while (!isAttackCoordinatesValid(attackCoordinates, cpu));

        char hitPos = cpu->applyImpact(attackCoordinates);
        draw(human, cpu);
        attackCoordinates.clear();
        winner = updateGameState(cpu, hitPos);

        do
        {
            attackCoordinates = cpu->attack();
        } while (!isAttackCoordinatesValid(attackCoordinates, human));

        hitPos = human->applyImpact(attackCoordinates);
        draw(human, cpu);
        winner = updateGameState(human, hitPos);

        Sleep(1000);
    }

    std::cout << winner << " wins!" << std::endl;
    delete human;
    delete cpu;

    Sleep(5000);
    system("cls");
    intro();
}

void draw(Player* playerOne, Player* playerTwo)
{
    system("cls");
    playerOne->draw();
    std::cout << std::endl;
    playerTwo->drawDisplayGrid(); // Cheated a bit since we know this is the CPU for single player...
}

bool isAttackCoordinatesValid(std::vector<int> attackCoordinates, Player* player)
{
    if (!attackCoordinates.size())
    {
        return false;
    }

    char pos = player->getGrid()[attackCoordinates[0]][attackCoordinates[1]];
    if (pos != 'X' && pos != 'O')
    {
        return true;
    }
    return false;
}

std::string updateGameState(Player* player, char posHit)
{
    std::string type = "";

    if (dynamic_cast<Human*>(player))
    {
        type = "Human";
    }
    else if (dynamic_cast<CPU*>(player))
    {
        type = "CPU";
    }

    for (Ship* ship : player->getShips())
    {
        if (posHit == ship->getSprite())
        {
            ship->setHits(ship->getHits() + 1);
            if (ship->getSpaces() == ship->getHits())
            {
                ship->setDestroyed(true);
                std::cout << type << "'s " << ship->getType() << " was destroyed!" << std::endl;
                gameOver = checkWinStatus(player);
            }
        }
    }
    return type;
}

bool checkWinStatus(Player* player)
{
    for (Ship* ship : player->getShips())
    {
        if (!ship->isDestroyed())
        {
            return false;
        }
    }
    return true;
}