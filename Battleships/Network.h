#pragma once
#include "Human.h"
#include <boost/asio.hpp>
class Network
{
private:
	boost::asio::io_service io_service;
	std::string ip;
	int port;
public:
	Network(std::string ip, int port);
	~Network();
	char sendAttack(std::vector<int> & attackCoordinates, Human* player);
	char receiveAttack(Human* player);
	void sendGameOver();
};

