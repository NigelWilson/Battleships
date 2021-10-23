#pragma once
#include <string>
class Ship
{
private:
	std::string type;
	char sprite;
	int spaces;
	int x;
	int y;
	bool destroyed;
	int hits;
public:
	Ship(std::string type, char sprite, int spaces);
	~Ship();

	std::string getType();
	void setType(std::string type);

	char getSprite();
	void setSprite(char sprite);

	int getSpaces();
	void setSpaces(int spaces);

	int getX();
	void setX(int x);

	int getY();
	void setY(int y);

	bool isDestroyed();
	void setDestroyed(bool destroyed);

	int getHits();
	void setHits(int hits);
};

