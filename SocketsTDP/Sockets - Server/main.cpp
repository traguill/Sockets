/* SERVER */

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include "Windows.h"
#include "WinSock2.h"
#include "Ws2tcpip.h"

#include <stdio.h>
#include <stdlib.h>

int Quit()
{
	Sleep(2000);
	return 0;
}

bool SendResponse(SOCKET s)
{
	const char* msg = "pong";
	int send_result = send(s, msg, sizeof(msg), 0);

	if (send_result == SOCKET_ERROR)
	{
		printf("Server: sent failed - %d\n", WSAGetLastError());
		return true;
	}
	return false;
}

int main(int argc, char* argv[])
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

	//Bind
	sockaddr_in bind_address;
	bind_address.sin_family = AF_INET; //IPv4
	bind_address.sin_port = htons(port_address);
	inet_pton(AF_INET, ip_address, &bind_address.sin_addr);
	
	bind(s, (SOCKADDR*)&bind_address, sizeof(sockaddr_in));

	//Listen
	listen(s, 5);

	//Accept
	

	char buffer[512];
	int length = 512;
	int recv_result;

	bool error = false;

	sockaddr_in send_addr;
	int send_size = sizeof(send_addr);

	SOCKET s2 = accept(s, (SOCKADDR*)&send_addr, &send_size);

	const char* msg_buff = "pong";

	//Receive
	do 
	{
		recv_result = recv(s2, buffer, length, 0);
		if (recv_result > 0 && recv_result <= length)
		{
			printf("Server: message received: %s", buffer);
			error = SendResponse(s2);
		}
		else if (recv_result == 0 && length > 0)
			error = true; //end
		else
			printf("Server: recv failed: %d\n", WSAGetLastError());
	} while (error == false);
	
	
	Quit();
	//Clean Up socket library
	closesocket(s);
	closesocket(s2);
	result = WSACleanup();

	return 0;
}

/*

	TCP CODE
	do
	{
	recv_result = recv(s, buffer, length, 0);
	if (recv_result > 0)
	printf("Bytes received by the server: %d", recv_result);
	else if (recv_result == 0 && length == 0)
	printf("Server: There's still info to receive\n");
	else if (recv_result == 0 && length > 0)
	{
	printf("Server: message received: %s", buffer);
	error = SendResponse(s);
	}
	else
	printf("Server: recv failed: %d\n", WSAGetLastError());
	} while (recv_result > 0 && error == false);


	const char* msg = "pong";
	int send_result = send(s, msg, sizeof(msg), 0);

	if (send_result == SOCKET_ERROR)
	{
	printf("Server: sent failed - %d\n", WSAGetLastError());
	return true;
	}
*/
