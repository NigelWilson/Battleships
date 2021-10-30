#include "Ship.h"

Ship::Ship(std::string type, char sprite, int spaces)
{
	this->setType(type);
	this->setSprite(sprite);
	this->setSpaces(spaces);
	this->setDestroyed(false);
	this->setHits(0);
}

Ship::~Ship(){}

std::string Ship::getType()
{
	return this->type;
}

void Ship::setType(std::string type)
{
	this->type = type;
}

char Ship::getSprite()
{
	return this->sprite;
}

void Ship::setSprite(char sprite)
{
	this->sprite = sprite;
}

int Ship::getSpaces()
{
	return this->spaces;
}

void Ship::setSpaces(int spaces)
{
	this->spaces = spaces;
}

int Ship::getX()
{
	return this->x;
}

void Ship::setX(int x)
{
	this->x = x;
}

int Ship::getY()
{
	return this->y;
}

void Ship::setY(int y)
{
	this->y = y;
}

bool Ship::isDestroyed()
{
	return this->destroyed;
}

void Ship::setDestroyed(bool destroyed)
{
	this->destroyed = destroyed;
}

int Ship::getHits()
{
	return this->hits;
}

void Ship::setHits(int hits)
{
	this->hits = hits;
}
