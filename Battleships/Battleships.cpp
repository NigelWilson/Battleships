// Battleships.cpp : This file contains the 'main' function. Program execution begins and ends there.
#include "Ship.h"
#include "Human.h"
#include "CPU.h"
#include "Network.h"
#include <iostream>
#include <string>
#include <Windows.h>
#include <vector>
#include <typeinfo>

void draw(Player* playerOne, Player* playerTwo);
void drawMultiplayer(Player* player);
bool isAttackCoordinatesValid(std::vector<int> attackCoordinates, Player* player, bool isMultiplayer);
std::string updateGameState(Player* player, char hitPos);
bool checkWinStatus(Player* player);
void intro();
void playSinglePlayer();
void playMultiplayer();

bool gameOver = false;
std::string winner = "";
std::vector<int> attackCoordinates;

int main()
{
    intro();
}

void intro()
{
    gameOver = false;
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
        playMultiplayer();
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
        } while (!isAttackCoordinatesValid(attackCoordinates, cpu, false));

        char hitPos = cpu->applyImpact(attackCoordinates, false, false);
        draw(human, cpu);
        attackCoordinates.clear();
        winner = updateGameState(cpu, hitPos);

        do
        {
            attackCoordinates = cpu->attack();
        } while (!isAttackCoordinatesValid(attackCoordinates, human, false));

        hitPos = human->applyImpact(attackCoordinates, false, false);
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

void playMultiplayer()
{
    //https://www.boost.org/doc/libs/1_44_0/doc/html/boost_asio/examples.html
    //https://www.programmingnotes.org/4640/c-snippet-how-to-send-text-over-a-network-using-a-tcp-connection/
    //https://www.c-sharpcorner.com/article/socket-programming-in-cpp-using-boost-asio-tcp-server-and-client/ looks best!

    Human* player = new Human();

    system("cls");
    std::cout << "Are you client or server?" << std::endl;
    std::cout << "1. Client" << std::endl;
    std::cout << "2. Server" << std::endl;

    std::string input;
    std::cin >> input;

    if (std::stoi(input) == 1)
    {
        Network* p1Network = new Network();
        //p1Network->send();

        player->addShips();
        drawMultiplayer(player);

        int count = 1;

        while (!gameOver)
        {
            if (count % 2 != 0)
            {
                do
                {
                    attackCoordinates = player->attack();
                    
                } while (!isAttackCoordinatesValid(attackCoordinates, player, true));
                
                p1Network->sendAttack(attackCoordinates, player);
                char hitPos = player->applyImpact(attackCoordinates, true, true);
                drawMultiplayer(player);
                attackCoordinates.clear();
                //winner = updateGameState(player, hitPos);
            }
            else
            {
                std::cout << "Waiting for enemy attack..." << std::endl;
                char hitPos = p1Network->receiveAttack(player);

                // If attack last turn won the game, other player will automatically attack with 'g'
                if (hitPos == 'g')
                {
                    gameOver = true;
                    delete p1Network;
                    continue;
                }

                //char hitPos = player->applyImpact(attackCoordinates, true);
                drawMultiplayer(player);
                attackCoordinates.clear();
                winner = updateGameState(player, hitPos);
                
                if (gameOver)
                {
                    p1Network->sendGameOver();
                    delete p1Network;
                }
            }
            count++;
        }
    }
    else if (std::stoi(input) == 2)
    {
        Network* p2Network = new Network();
        //p2Network->receive();

        player->addShips();
        drawMultiplayer(player);

        int count = 1;

        while (!gameOver)
        {
            if (count % 2 == 0)
            {
                do
                {
                    attackCoordinates = player->attack();

                } while (!isAttackCoordinatesValid(attackCoordinates, player, true));
                p2Network->sendAttack(attackCoordinates, player);
                char hitPos = player->applyImpact(attackCoordinates, true, true);
                drawMultiplayer(player);
                attackCoordinates.clear();
                //winner = updateGameState(player, hitPos);
            }
            else
            {
                std::cout << "Waiting for enemy attack..." << std::endl;
                char hitPos = p2Network->receiveAttack(player);

                // If attack last turn won the game, other player will automatically attack with 'g'
                if (hitPos == 'g')
                {
                    gameOver = true;
                    delete p2Network;
                    continue;
                }

                //player->applyImpact(attackCoordinates, true);
                drawMultiplayer(player);
                attackCoordinates.clear();
                winner = updateGameState(player, hitPos);

                if (gameOver)
                {
                    p2Network->sendGameOver();
                    delete p2Network;
                }

            }
            count++;
        }
    }
    else
    {
        return;
    }

    std::cout << winner << " wins!" << std::endl;
    delete player;

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

void drawMultiplayer(Player* player)
{
    system("cls");
    std::cout << "Enemy Grid" << std::endl;
    player->drawDisplayGrid();
    std::cout << std::endl;
    std::cout << "Your Grid" << std::endl;
    player->draw();
}

bool isAttackCoordinatesValid(std::vector<int> attackCoordinates, Player* player, bool isMultiplayerGame)
{
    if (!attackCoordinates.size())
    {
        return false;
    }

    char pos;

    if (isMultiplayerGame)
    {
        pos = player->getDisplayGrid()[attackCoordinates[0]][attackCoordinates[1]];
    }
    else
    {
        pos = player->getGrid()[attackCoordinates[0]][attackCoordinates[1]];
    }
    
    if (pos != 'X' && pos != 'O')
    {
        return true;
    }
    return false;
}

// Turns out this method is tracking the game state for both players in a single set of ships for multiplayer
// Need to ensure it's only one set.
// Also need to ensure that the enemy player gets informed of a gameOver state
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