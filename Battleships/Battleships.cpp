#include "Ship.h"
#include "Human.h"
#include "CPU.h"
#include "Network.h"
#include <iostream>
#include <string>
#include <vector>
#include <boost/asio.hpp>

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
std::vector<std::string> logText;

int main()
{
    intro();
}

void intro()
{
    gameOver = false;
    std::string input;
    int choice = 0;
    
    do
    {
        system("cls");

        std::cout << "Welcome to Battleships!" << std::endl;
        std::cout << std::endl;
        std::cout << "Please choose a game type:" << std::endl;
        std::cout << "1. Single Player" << std::endl;
        std::cout << "2. Multiplayer" << std::endl;
        std::cout << "3. Quit" << std::endl;

        std::cin >> input;

        if (input.find_first_not_of("123") != std::string::npos)
        {
            continue;
        }

        choice = std::stoi(input);

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
    } while (choice != 1 && choice != 2 && choice != 3);
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

        char hitPos = cpu->applyImpact(attackCoordinates, false, false, NULL);
        attackCoordinates.clear();
        winner = updateGameState(cpu, hitPos);
        draw(human, cpu);

        do
        {
            attackCoordinates = cpu->attack();
        } while (!isAttackCoordinatesValid(attackCoordinates, human, false));

        hitPos = human->applyImpact(attackCoordinates, false, false, NULL);

        if (hitPos != ' ')
        {
            cpu->addHit(attackCoordinates[0], attackCoordinates[1]);
        }

        winner = updateGameState(human, hitPos);
        draw(human, cpu);

        Sleep(1000);
    }

    std::cout << winner << " wins!" << std::endl;
    delete human;
    delete cpu;

    // Not great practice to use these as they run OS level apps on the path
    system("pause");
    system("cls");
    intro();
}

void playMultiplayer()
{
    Human* player = new Human();
    std::map<char, int> hitTracker{ {'C', 0}, {'B', 0}, {'D', 0}, {'S', 0}, {'P', 0} };

    system("cls");
    std::cout << "Are you client or server?" << std::endl;
    std::cout << "1. Client" << std::endl;
    std::cout << "2. Server" << std::endl;

    std::string input;
    std::cin >> input;

    std::string ip = "";
    std::string inputPort;
    int port = 0;
    boost::asio::ip::address address;
    boost::system::error_code ec;
    
    if (std::stoi(input) == 1)
    {
        do
        {
            system("cls");
            std::cout << "Please enter the IP you will be connecting to" << std::endl;
            std::cin >> ip;
            address = boost::asio::ip::address::from_string(ip, ec);
        } while (ec || !address.is_v4());
    }

    do
    {
        system("cls");
        std::cout << "Please enter the port number you will be using/connecting to (between 1000 and 65535)" << std::endl;
        std::cin >> inputPort;
        if (!inputPort.empty() && inputPort.find_first_not_of("0123456789") == std::string::npos)
        {
            port = std::stoi(inputPort);
        }
    } while (port < 1000 || port > 65535);

    system("cls");
    Network* network = new Network(ip, port);
    player->addShips();
    drawMultiplayer(player);

    int turn = 1;

    while (!gameOver)
    {
        if (std::stoi(input) == 1 && turn % 2 != 0 || std::stoi(input) == 2 && turn % 2 == 0)
        {
            do
            {
                attackCoordinates = player->attack();

            } while (!isAttackCoordinatesValid(attackCoordinates, player, true));
            char response = network->sendAttack(attackCoordinates, player);

            if (response == 'f')
            {
                break;
            }

            std::map<char, int>::iterator it = hitTracker.find(response);
            if (it != hitTracker.end())
            {
                it->second++;

                for (Ship* ship : player->getShips())
                {
                    if (ship->getSprite() == response && ship->getSpaces() == it->second)
                    {
                        logText.push_back("Enemy " + ship->getType() + " destroyed!");
                    }
                }
            }

            char hitPos = player->applyImpact(attackCoordinates, true, true, response);
            drawMultiplayer(player);
            attackCoordinates.clear();
        }
        else
        {
            std::cout << "Waiting for enemy attack..." << std::endl;
            char hitPos = network->receiveAttack(player);

            // If attack last turn won the game, other player will automatically attack with 'g'
            if (hitPos == 'g')
            {
                gameOver = true;
                delete network;
                winner = "You";
                continue;
            }

            winner = updateGameState(player, hitPos);
            drawMultiplayer(player);
            attackCoordinates.clear();

            if (gameOver)
            {
                network->sendGameOver();
                delete network;
                winner = "Enemy";
            }
        }
        turn++;
    }

    if (!winner.empty())
    {
        std::cout << winner << " won!" << std::endl;
    }

    delete player;

    system("pause");
    system("cls");
    intro();
}

void draw(Player* playerOne, Player* playerTwo)
{
    system("cls");
    std::cout << "CPU Grid" << std::endl;
    playerTwo->drawDisplayGrid(); // Cheated a bit since we know this is the CPU for single player...
    std::cout << std::endl;
    playerOne->draw();
    std::cout << std::endl;

    for (std::string s : logText)
    {
        std::cout << s << std::endl;
    }
    std::cout << std::endl;
}

void drawMultiplayer(Player* player)
{
    system("cls");
    std::cout << "Enemy Grid" << std::endl;
    player->drawDisplayGrid();
    std::cout << std::endl;
    player->draw();
    std::cout << std::endl;

    for (std::string s : logText)
    {
        std::cout << s << std::endl;
    }
    std::cout << std::endl;
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

std::string updateGameState(Player* player, char posHit)
{
    std::string type = "";

    if (dynamic_cast<Human*>(player))
    {
        type = "Player";
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
                logText.push_back(type + "'s " + ship->getType() + " destroyed!");
                gameOver = checkWinStatus(player);
            }
        }
    }
    return type == "Player" ? "CPU" : "Player";
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