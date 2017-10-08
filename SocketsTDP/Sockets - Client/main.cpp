/* CLIENT */

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include "Windows.h"
#include "WinSock2.h"
#include "Ws2tcpip.h"

#include <stdio.h>
#include <stdlib.h>

int Quit()
{
	Sleep(10000);
	return 0;
}

int main(int argc, char **argv)
{
	//Retrieve Arguments
	if (argc != 3)
	{
		printf("Not enough parameters to initialize the server. Pass the ip address and the port.");
		return Quit();
	}

	const char* ip_address = argv[1];
	int port_address = atoi(argv[2]);

	//Init socket library
	WSADATA wsa_data;
	int result = WSAStartup(MAKEWORD(2, 2), &wsa_data);
	if (result != NO_ERROR)
	{
		printf("Socket library could not initialize");
		return Quit();
	}

	//Create the socket
	//SOCKET s = socket(AF_INET, SOCK_DGRAM, 0); //UDP
	SOCKET s = socket(AF_INET, SOCK_STREAM, 0); //TCP

	char buffer[512];
	int length = 512;
	int recv_result;

	bool error = false;

	sockaddr_in send_addr;
	send_addr.sin_family = AF_INET; //IPv4
	send_addr.sin_port = htons(port_address);
	inet_pton(AF_INET, ip_address, &send_addr.sin_addr);
	int send_size = sizeof(send_addr);

	//Connect
	recv_result = connect(s, (SOCKADDR*)&send_addr, send_size);
	if (recv_result == SOCKET_ERROR)
	{
		printf("Client: error while connecting %d\n", WSAGetLastError());
		return Quit();
	}

	const char* msg_buff = "ping";
	int count = 0;
	do
	{
		int send_result = send(s, msg_buff, sizeof(msg_buff), 0);

		if (send_result == SOCKET_ERROR)
		{
			printf("Client: sent failed - %d\n", WSAGetLastError());
			error = true;
		}
		else
		{
			++count;
			recv_result = recv(s, buffer, length, 0);
			if (recv_result > 0)
				printf("Bytes received by the server: %d", recv_result);
			else if (recv_result == 0 && length == 0)
				printf("Client: There's still info to receive\n");
			else if (recv_result != 0 || length <= 0)
			{
				printf("Client: recv failed: %d\n", WSAGetLastError());
				error = true;

			}
		}
	} while (count < 5 && error == false);


	Quit();

	//Clean Up socket library
	closesocket(s);
	result = WSACleanup();

	return 0;
}