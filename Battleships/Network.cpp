#include "Network.h"
#include <iostream>
#include <sstream>
#include <boost/asio.hpp>

using namespace boost::asio;
using ip::tcp;

Network::Network() : io_service() {}//, acceptor(io_service, tcp::endpoint(tcp::v4(), 8080)) {}//,socket(io_service){}
Network::~Network(){}

char Network::sendAttack(std::vector<int> & attackCoordinates, Human* player)
{
	tcp::socket socket(io_service);
	socket.connect(tcp::endpoint(boost::asio::ip::address::from_string("127.0.0.1"), 8080));

	//const std::string msg = "Hello from Client!\n";
	const std::string msg = std::to_string(attackCoordinates[0]) + ':' + std::to_string(attackCoordinates[1]) + "\n";
	boost::system::error_code error;
	boost::asio::write(socket, boost::asio::buffer(msg), error);

	if (!error)
	{
		std::cout << "Successfully sent message from client to server!" << std::endl;
	}
	else
	{
		std::cout << "Failed: " << error.message() << std::endl;
	}

	boost::asio::streambuf receive_buffer;
	boost::asio::read_until(socket, receive_buffer, "\n", error);
	//boost::asio::read(socket, receive_buffer, boost::asio::transfer_all(), error);
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
	tcp::acceptor acceptor(io_service, tcp::endpoint(tcp::v4(), 8080));
	//waiting for the connection
	acceptor.accept(socket);
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

	char hitPos = player->applyImpact(received, true, false);

	//write operation
	boost::system::error_code error;
	//boost::asio::write(socket, boost::asio::buffer("Hi from Server!\n"), error);
	boost::asio::write(socket, boost::asio::buffer(std::string(1, hitPos) + "\n"), error);
	acceptor.close();
	socket.close();

	if (error && error != boost::asio::error::eof)
	{
		std::cout << "receive failed: " << error.message() << std::endl;
	}
	else
	{
		std::cout << "Apparently successfully sent reply to the client" << std::endl;
	}

	return hitPos;
}

void Network::sendGameOver()
{
	tcp::socket socket(io_service);
	socket.connect(tcp::endpoint(boost::asio::ip::address::from_string("127.0.0.1"), 8080));

	const std::string msg = "gameover\n";
	boost::system::error_code error;
	boost::asio::write(socket, boost::asio::buffer(msg), error);

	if (!error)
	{
		std::cout << "Successfully sent message from client to server!" << std::endl;
	}
	else
	{
		std::cout << "Failed: " << error.message() << std::endl;
	}
}