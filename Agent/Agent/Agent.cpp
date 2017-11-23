// Agent.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

int main()
{
	//1. initializing socket
	boost::asio::io_service service;
	boost::asio::ip::tcp::endpoint ep(boost::asio::ip::address::from_string("127.0.0.1"), 8000);
	boost::asio::ip::tcp::socket sock(service);

	boost::system::error_code  ec;
	sock.connect(ep, ec);
	if (ec)
	{
		cout << "Failed to connect to the server." << endl;
		return -1;
	}
	else
		cout << "Conected succesfully." << endl;

	//2. session
	for (;;)
	{
		Sleep(1000);
		if (sock.is_open())
		{
			//2.1 make a screenshot and getting its name
			string fileName = SaveScreenShot();
			fstream fin(fileName, ios::in | ios::binary);// opening screen in binary format to send on server

			//2.2 creating json structure and reading file in ostringstream to get data and size of the file
			pt::ptree root;//element wich will be used to create a json string for request
			

			ostringstream dataStream;
			dataStream << fin.rdbuf();
			string data = dataStream.str();

			fillPTree(root, "client", data.length());
			//2.3 writing data of json structure into the string
			ostringstream requestStream;
			boost::property_tree::json_parser::write_json(requestStream, root);
			string sRequest = requestStream.str();

			//2.4 sending on a server
			boost::asio::write(sock, boost::asio::buffer(sRequest, sRequest.length()));

			//2.5 getting responce
			char answer[256] = "";
			sock.read_some(boost::asio::buffer(answer, sizeof(answer)));

			//2.6 working with responce
			int a = strcmp(answer, "00");

			if (a == 0)
			{
				cout << "Right responce - sending a file..." << endl;
				//writing data string on server
				boost::asio::write(sock, boost::asio::buffer(data, sizeof(data)));
				cout << "File is sent." << endl;
			}
			else
			{
				cout << "Uncorrect responce - file will not be sent." << endl;

			}

			//2.7 closing fstream object
			fin.close();

		}
		else
		{
			cout << "Connection error aquired" << endl;
		}

	}


	//3. closing socket 
	sock.close();

	return 0;
}
