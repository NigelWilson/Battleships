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
void drawLogText();
bool isAttackCoordinatesValid(std::vector<int> attackCoordinates, Player* player, bool isMultiplayer);
std::string updateGameState(Player* player, char hitPos);
bool checkWinStatus(Player* player);
void intro();
void playSinglePlayer();
void playMultiplayer();
void displayRules();
std::string getClientOrServer();
std::string getIP();
int getPort();

bool gameOver = false;
std::string winner = "";
std::vector<int> attackCoordinates;
std::vector<std::string> logText;

int main()
{
    intro();
}

// Displays a main menu and allows user to select game type
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
        std::cout << "3. How to Play" << std::endl;
        std::cout << "4. Quit" << std::endl;

        std::cin >> input;

        if (input.find_first_not_of("1234") != std::string::npos)
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
        else if (std::stoi(input) == 3)
        {
            displayRules();
        }
        else
        {
            return;
        }
    } while (choice != 1 && choice != 2 && choice != 3);
}

// Sets up player objects for human and CPU and executes single player game logic
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

        // Impossible for human to win without this since updateGameState()
        // below for CPU's turn will override gameOver state
        if (gameOver) break;

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
    logText.clear();
    intro();
}

// Sets up required objects for network multiplayer game and executes game logic
void playMultiplayer()
{
    Human* player = new Human();
    // Allows us to track hits we have applied to the enemy player
    std::map<char, int> hitTracker{ {'C', 0}, {'B', 0}, {'D', 0}, {'S', 0}, {'P', 0} };
    std::string input = getClientOrServer();
    
    std::string ip = "";
    if (std::stoi(input) == 1) ip = getIP();

    int port = getPort();
    
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

            if (response == 'f') break;

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
    logText.clear();
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
    drawLogText();
}

void drawMultiplayer(Player* player)
{
    system("cls");
    std::cout << "Enemy Grid" << std::endl;
    player->drawDisplayGrid();
    std::cout << std::endl;
    player->draw();
    std::cout << std::endl;
    drawLogText();
}

// Display log of destroyed ships
void drawLogText()
{
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
    // If type == Player it was CPU that attacked this turn, so winner would be CPU on gameOver
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

/*
    Helper methods to get user input for network multiplayer games
*/
std::string getClientOrServer()
{
    std::string input;
    do
    {
        system("cls");
        std::cout << "Are you client or server?" << std::endl;
        std::cout << "1. Client" << std::endl;
        std::cout << "2. Server" << std::endl;
        std::cin >> input;
    } while (input.find_first_not_of("12") != std::string::npos);

    return input;
}

std::string getIP()
{
    boost::asio::ip::address address;
    boost::system::error_code ec;
    std::string ip = "";

    do
    {
        system("cls");
        std::cout << "Please enter the IP you will be connecting to" << std::endl;
        std::cin >> ip;
        address = boost::asio::ip::address::from_string(ip, ec);
    } while (ec || !address.is_v4());

    return ip;
}

int getPort()
{
    std::string inputPort;
    int port = 0;
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

    return port;
}

void displayRules()
{
    system("cls");
    std::cout << "**************************************" << std::endl;
    std::cout << "How to Play Battleships" << std::endl;
    std::cout << "**************************************" << std::endl;
    std::cout << std::endl;
    std::cout << "Battleships is a game where each player as an 8x8 square grid on which five ships must be placed." << std::endl;
    std::cout << "Once all the ships have been placed by you and your opponent, each player will take turns to select a coordinate of the enemies grid to attack." << std::endl;
    std::cout << "If the attack hits an enemy ship, that space will display as an X, if not a O will fill that space. If you land a hit, try attacking nearby ";
    std::cout << "spaces to completely destroy an enemy ship." << std::endl;
    std::cout << "The game is over once all ships from one player have been destroyed." << std::endl;

    std::cout << std::endl;
    std::cout << std::endl;

    std::cout << "**************************************" << std::endl;
    std::cout << "Ships" << std::endl;
    std::cout << "**************************************" << std::endl;
    std::cout << std::endl;
    std::cout << "Carrier: Uses 5 grid spaces - represented by C" << std::endl;
    std::cout << "Battleship: Uses 4 grid spaces - represented by B" << std::endl;
    std::cout << "Destroyer: Uses 3 grid spaces - represented by D" << std::endl;
    std::cout << "Submarine: Uses 3 grid spaces - represented by S" << std::endl;
    std::cout << "Patrol Boat: Uses 2 grid spaces - represented by P" << std::endl;

    std::cout << std::endl;
    std::cout << std::endl;

    std::cout << "**************************************" << std::endl;
    std::cout << "How do I place ships on my grid?" << std::endl;
    std::cout << "**************************************" << std::endl;
    std::cout << std::endl;
    std::cout << "To place ships you must enter the starting and ending coordinate you wish to use for each ship, separated by a colon character." << std::endl;
    std::cout << "For example, placing a Carrier which must take up 5 spaces you could enter 'A1:A5' or 'A1:E1'." << std::endl;

    std::cout << std::endl;
    std::cout << std::endl;

    std::cout << "**************************************" << std::endl;
    std::cout << "How do I attack the enemy?" << std::endl;
    std::cout << "**************************************" << std::endl;
    std::cout << std::endl;
    std::cout << "Simply enter the coordinates by column and row you wish to attack. For example you could enter 'A1' to attack that space." << std::endl;

    std::cout << std::endl;
    std::cout << std::endl;

    std::cout << "**************************************" << std::endl;
    std::cout << "How does Multiplayer work?" << std::endl;
    std::cout << "**************************************" << std::endl;
    std::cout << std::endl;
    std::cout << "Firstly select whether you are hosting the game or connecting to an existing host. The host should choose to be the server, the other player should select client." << std::endl;
    std::cout << "If your are the client, you must enter the IP address of the opposing player. If playing on LAN this will be the local IP address of their device, ";
    std::cout << "if playing online it will need the external IP address of the opposing players network. They can find this by googling 'what is my ip'. ";
    std::cout << "Additionally if playing online, both players will need to set up port forwarding on their respective routers to ensure network traffic reaches the device playing the game. ";
    std::cout << "This port number must then be entered by both players when requested by the game on starting a multiplayer match." << std::endl;
    std::cout << "You may also need to allow the game to communicate over public networks. This should manifest as a pop up on starting a multiplayer game, or can be set in Windows Firewall ";
    std::cout << "settings." << std::endl;

    std::cout << std::endl;
    std::cout << std::endl;

    std::cout << "I hope you enjoy playing Battleships!" << std::endl;
    std::cout << std::endl;

    system("pause");
    intro();
}