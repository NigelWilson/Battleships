#pragma once
#include "Human.h"
#include <boost/asio.hpp>
class Network
{
public:
	Network(std::string ip);
	~Network();
	char sendAttack(std::vector<int> & attackCoordinates, Human* player);
	char receiveAttack(Human* player);
	void sendGameOver();
private:
	boost::asio::io_service io_service;
	//boost::asio::ip::tcp::acceptor acceptor;
	//boost::asio::ip::tcp::socket socket;
	std::string ip;
	int port;
};

