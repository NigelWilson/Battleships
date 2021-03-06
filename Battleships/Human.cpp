#include "Human.h"
#include <iostream>
#include <sstream>
#include <regex>

Human::Human(){}

Human::~Human(){}

void Human::draw()
{
	std::cout << "Player Grid" << std::endl;
	Player::draw();
}

void Human::addShips()
{
    const std::regex inputRegex("[A-H][1-8]:[A-H][1-8]");

    for (Ship* ship : this->getShips())
    {
        this->draw();
        int c1a = 0, c1b = 0, c2a = 0, c2b = 0;

        do
        {
            c1a = 0, c1b = 0; c2a = 0, c2b = 0;
            // Keep asking player for inputs until they are valid
            if (!inputError.empty()) {
                std::cout << inputError << std::endl;
                inputError = "";
            }

            std::cout << "Please enter coordianates for your " << ship->getType() << " separated by a colon e.g.A1:A5" << std::endl;
            std::cout << "You need " << ship->getSpaces() << " spaces to place your " << ship->getType() << std::endl;
            std::string input;
            std::cin >> input;

            std::cout << std::endl;

            if (!std::regex_match(input, inputRegex))
            {
                inputError = "One or more coordinates exceeds grid boundary!";
                continue;
            }

            std::vector<std::string> split;
            std::stringstream stream(input);
            std::string segment;

            while (std::getline(stream, segment, ':'))
            {
                split.push_back(segment);
            }

            std::string front = split.front();
            std::string back = split.back();

            c1a = alphaChars.at(front[0]);
            c1b = char(front[1]) - '0'; // returns ascii value, interesting! Subtract char '0' to get the actual char value as int
            c2a = alphaChars.at(back[0]);
            c2b = char(back[1]) - '0';
            
        } while (!validShipCoordinates(c1a, c1b, c2a, c2b, ship));

        // One of the axis must have the same value, check which one and then fill in the blanks on the other
        if (c1b == c2b)
        {
            // Need this condition in case people try to put the coordinates in reverse order
            if (c1a < c2a)
            {
                for (int i = c1a; i <= c2a; i++)
                {
                    this->modifyGrid(c1b, i, ship->getSprite());
                }
            }
            else
            {
                for (int i = c2a; i <= c1a; i++)
                {
                    this->modifyGrid(c1b, i, ship->getSprite());
                }
            }
        }
        else
        {
            if (c1b < c2b)
            {
                for (int i = c1b; i <= c2b; i++)
                {
                    this->modifyGrid(i, c2a, ship->getSprite());
                }
            }
            else
            {
                for (int i = c2b; i <= c1b; i++)
                {
                    this->modifyGrid(i, c2a, ship->getSprite());
                }
            }
        }
        system("cls");
    }
}

bool Human::validShipCoordinates(int c1a, int c1b, int c2a, int c2b, Ship* ship)
{
    if (c1a < 1 || c1a > 8 || c1b < 1 || c1b > 8 || c2a < 1 || c2a > 8 || c2b < 1 || c2b > 8)
    {
        inputError = "One or more coordinates exceeds grid boundary!";
        return false;
    }

    if (c1b == c2b)
    {
        // Are coordinates reversed or not? E.G A5:A1 as oposed to A1:A5
        if (c1a < c2a)
        {
            if ((c2a - c1a) + 1 != ship->getSpaces())
            {
                inputError = "Coordinates do not match required ship size!";
                return false;
            }

            for (int i = c1a; i <= c2a; i++)
            {
                if (this->getGrid()[c1b][i] != ' ')
                {
                    inputError = "One or more coordinates you tried to use is blocked!";
                    return false;
                }
            }
        }
        else
        {
            if ((c1a - c2a) + 1 != ship->getSpaces())
            {
                inputError = "Coordinates do not match required ship size!";
                return false;
            }

            for (int i = c2a; i <= c1a; i++)
            {
                if (this->getGrid()[c1b][i] != ' ')
                {
                    inputError = "One or more coordinates you tried to use is blocked!";
                    return false;
                }
            }
        }
    }
    else if (c1a == c2a)
    {
        if (c1b < c2b)
        {
            if ((c2b - c1b) + 1 != ship->getSpaces())
            {
                inputError = "Coordinates do not match required ship size!";
                return false;
            }

            for (int i = c1b; i <= c2b; i++)
            {
                if (this->getGrid()[i][c2a] != ' ')
                {
                    inputError = "One or more coordinates you tried to use is blocked!";
                    return false;
                }
            }
        }
        else
        {
            if ((c1b - c2b) + 1 != ship->getSpaces())
            {
                inputError = "Coordinates do not match required ship size!";
                return false;
            }

            for (int i = c2b; i <= c1b; i++)
            {
                if (this->getGrid()[i][c2a] != ' ')
                {
                    inputError = "One or more coordinates you tried to use is blocked!";
                    return false;
                }
            }
        }
    }
    else 
    {
        // Diagonals not allowed
        inputError = "Diagonals not allowed!";
        return false;
    }

    return true;
}

std::vector<int> Human::attack()
{
    int row = 0, col = 0;
    std::vector<int> attackCoordinates;
    const std::regex attackRegex("[A-H][1-8]");

    std::cout << "Input coordinates to attack: " << std::endl;
    std::string input;
    std::cin >> input;

    if (!std::regex_match(input, attackRegex))
    {
        return attackCoordinates;
    }

    try
    {
        attackCoordinates.push_back(char(input[1]) - '0');
        attackCoordinates.push_back(alphaChars.at(input[0]));
    }
    catch (std::out_of_range)
    {
        std::cout << "You gave rubbish inputs, game over!" << std::endl;
    }

    return attackCoordinates;
}
