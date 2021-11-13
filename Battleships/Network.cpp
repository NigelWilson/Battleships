#include "Network.h"
#include <iostream>
#include <sstream>
#include <boost/asio.hpp>

using namespace boost::asio;
using ip::tcp;

// Not sure why the commented constructor arguments didn't work effectively
// The socket in particular would be able to receive data but the sent response would never be received by the client.
Network::Network(std::string ip, int port) : io_service() //, acceptor(io_service, tcp::endpoint(tcp::v4(), 8080)) {}//,socket(io_service){}
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
	tcp::socket socket(io_service);
	std::unique_ptr<tcp::acceptor> acceptor;

	// Should limit requests to the address of the first connected client
	if (this->ip.empty())
	{
		acceptor = std::make_unique<tcp::acceptor>(io_service, tcp::endpoint(tcp::v4(), this->port));
	}
	else
	{
		acceptor = std::make_unique<tcp::acceptor>(io_service, tcp::endpoint(ip::address::from_string(this->ip), this->port));
	}

	//waiting for the connection
	acceptor->accept(socket);

	if (this->ip.empty())
	{
		this->ip = socket.remote_endpoint().address().to_string();
	}

	std::cout << socket.remote_endpoint().address().to_string() << std::endl;
	//read operation
	boost::asio::streambuf buf;
	boost::asio::read_until(socket, buf, "\n");
	std::string data = boost::asio::buffer_cast<const char*>(buf.data());
	std::cout << data << std::endl;

	if (data == "gameover\n") {
		return 'g';
	}

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
	boost::asio::write(socket, boost::asio::buffer(std::string(1, hitPos) + "\n"), error);
	//acceptor.close();
	acceptor->close();
	socket.close();

	if (error && error != boost::asio::error::eof)
	{
		std::cout << "receive failed: " << error.message() << std::endl;
	}

	return hitPos;
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