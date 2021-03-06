#include "Network.h"
#include <iostream>
#include <sstream>
#include <regex>
#include <boost/asio.hpp>

using namespace boost::asio;
using ip::tcp;

Network::Network(std::string ip, int port) : io_service()
{
	this->ip = ip;
	this->port = port;
}
Network::~Network(){}

char Network::sendAttack(std::vector<int> & attackCoordinates, Human* player)
{
	tcp::socket socket(io_service);
	bool connected = false;
	int attempts = 1;

	do
	{
		try
		{
			socket.connect(tcp::endpoint(boost::asio::ip::address::from_string(this->ip), this->port));
			connected = true;
		}
		catch (const boost::system::system_error& e)
		{
			std::cout << "Error connecting to other player, retrying: attempt " << attempts << std::endl;
			attempts++;

			if (attempts == 10)
			{
				std::cout << "Error connecting to other player: " << e.what() << std::endl;
				std::cout << "Returning to main menu..." << std::endl;
				return 'f';
			}
			Sleep(1000);
		}
	} while (!connected && attempts < 10);

	const std::string msg = std::to_string(attackCoordinates[0]) + ':' + std::to_string(attackCoordinates[1]) + "\n";
	boost::system::error_code error;
	boost::asio::write(socket, boost::asio::buffer(msg), error);

	if (error)
	{
		std::cout << "Failed: " << error.message() << std::endl;
	}

	boost::asio::streambuf receive_buffer;
	boost::asio::read_until(socket, receive_buffer, "\n", error);
	socket.close();

	if (error && error != boost::asio::error::eof)
	{
		std::cout << "receive failed: " << error.message() << std::endl;
		return ' '; // May cause problems if we reach here
	}
	else
	{
		const char* data = boost::asio::buffer_cast<const char*>(receive_buffer.data());
		std::cout << data << std::endl;
		return data[0];
	}
}

char Network::receiveAttack(Human* player)
{
	tcp::acceptor acceptor(io_service, tcp::endpoint(tcp::v4(), this->port));
	const std::regex attackRegex("[1-8]:[1-8]\\n");
	std::string data;

	do
	{
		if (!data.empty())
		{
			this->ip = "";
			data = "";
		}

		tcp::socket socket(io_service);

		try
		{
			acceptor.accept(socket);
		}
		catch (const boost::system::system_error& e)
		{
			std::cout << e.what() << std::endl;
		}
		
		if (this->ip.empty())
		{
			this->ip = socket.remote_endpoint().address().to_string();
		}
		//std::cout << socket.remote_endpoint().address().to_string() << std::endl;

		//read operation
		boost::asio::streambuf buf;
		boost::asio::read_until(socket, buf, "\n");
		data = boost::asio::buffer_cast<const char*>(buf.data());
		//std::cout << data << std::endl;

		if (data == "gameover\n") return 'g';

		// Handles bogus connections
		if (!std::regex_match(data, attackRegex)) continue;

		std::stringstream stream(data);
		std::string s;
		std::vector<int> received;
		while (std::getline(stream, s, ':'))
		{
			received.push_back(std::stoi(s));
		}

		char hitPos = player->applyImpact(received, true, false, NULL);

		//write operation
		boost::system::error_code error;
		std::cout << socket.is_open() << std::endl;
		boost::asio::write(socket, boost::asio::buffer(std::string(1, hitPos) + "\n"), error);
		acceptor.close();
		socket.close();

		if (error && error != boost::asio::error::eof)
		{
			std::cout << "receive failed: " << error.message() << std::endl;
		}

		return hitPos;
	} while (true);
}

void Network::sendGameOver()
{
	tcp::socket socket(io_service);
	socket.connect(tcp::endpoint(boost::asio::ip::address::from_string(this->ip), this->port));

	const std::string msg = "gameover\n";
	boost::system::error_code error;
	boost::asio::write(socket, boost::asio::buffer(msg), error);

	if (error)
	{
		std::cout << "Failed: " << error.message() << std::endl;
	}
}