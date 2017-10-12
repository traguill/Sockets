#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include "Windows.h"
#include "WinSock2.h"
#include "Ws2tcpip.h"

#include <list>
#include <cstdlib>
#include <iostream>

// Globals
SOCKET serverSocket; // Listen socket
std::list<SOCKET> sockets; // All connected sockets

void logSocketErrorAndExit(const char *msg)
{
	wchar_t *s = NULL;
	FormatMessageW(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL, WSAGetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPWSTR)&s, 0, NULL);
	fprintf(stderr, "%s: %S\n", msg, s);
	LocalFree(s);
	system("pause");
	exit(-1);
}

void handleIncomingData()
{
	// Input buffer
	const int inputBufferLen = 1300;
	char inputBuffer[inputBufferLen];

	// Create a new socket set
	// TODO
	fd_set readSet;
	FD_ZERO(&readSet);

	// Fill the set
	for (auto s : sockets) {
		// TODO
		FD_SET(s, &readSet);
	}

	// Call select
	// TODO
	timeval timeout;
	timeout.tv_sec = 0;
	timeout.tv_usec = 0;
	int sockets_available = select(0, &readSet, nullptr, nullptr, &timeout);

	// List to mark disconnected sockets
	std::list<SOCKET> disconnectedSockets;

	// Read selected sockets
	for (auto s : sockets)
	{
		// Check if s is ready for read
		if (FD_ISSET(s, &readSet)) {

			// Is the server socket ?
			if (s == serverSocket) {
				// Accept a new connection
				// TODO
				sockaddr_in s_addres;
				int s_size = sizeof(s_addres);
				SOCKET s2 = accept(s, (SOCKADDR*)&s_addres, &s_size);
				// Add the new socket to our list sockets
				// TODO
				sockets.push_back(s2);
			}

			// Is a client socket
			else {

				// Call recv
				// TODO
				int bytes_recv = recv(s, inputBuffer, inputBufferLen, 0);
				if (bytes_recv == SOCKET_ERROR)
				{
					int lastError = WSAGetLastError();
					if (lastError == WSAECONNRESET)
					{
						std::cout << "A client disconnected forciblyd\n";
						disconnectedSockets.push_back(s);
					}
				}
				else //Success
				{
					if (bytes_recv == 0)
					{
						std::cout << "A client disconnected successfully\n";
						disconnectedSockets.push_back(s);
					}
					else
					{
						std::cout << "Message received: " << inputBuffer << "\n";
					}
				}
				// Handle errors
				// - WSAEWOULDBLOCK (do nothing)
				// - WSAECONNRESET (client disconnected forcibly, so insert s into disconnectedSockets)

				// Handle graceful client disconnection (recv returned 0, so insert s into disconnectedSockets)

				// On success, log the received message
			}
		}
	}

	// Remove disconnected sockets
	for (auto s : disconnectedSockets) {
		sockets.remove(s);
	}
}

void handleOutgoingData()
{
	// Output buffer
	const char *outputBuffer = "Update state packet";
	const int outputBufferLen = strlen(outputBuffer) + 1;

	// Create a new socket set
	// TODO
	fd_set sendSet;
	FD_ZERO(&sendSet);

	// Fill the set
	for (auto s : sockets) {
		// TODO
		FD_SET(s, &sendSet);
	}

	// Call select
	// TODO
	timeval timeout;
	timeout.tv_sec = 0;
	timeout.tv_usec = 0;
	int sockets_available = select(0, nullptr, &sendSet, nullptr, &timeout);

	// List to mark disconnected sockets
	std::list<SOCKET> disconnectedSockets;

	// Send data to selected sockets
	for (auto s : sockets)
	{
		// Check if s is ready for send
		if (FD_ISSET(s, &sendSet)) {

			// TODO:

			// Call send

			// Handle errors
			// - WSAEWOULDBLOCK (do nothing)
			// - WSAECONNRESET (client disconnected, so insert s into disconnectedSockets)

			int bytes_send = send(s, outputBuffer, outputBufferLen, 0);
			if (bytes_send == SOCKET_ERROR)
			{
				int lastError = WSAGetLastError();
				if (lastError == WSAECONNRESET)
				{
					disconnectedSockets.push_back(s);
				}
			}
		}
	}

	// Remove disconnected sockets
	for (auto s : disconnectedSockets) {
		sockets.remove(s);
	}
}

void server(int port)
{
	// Startup
	WSAData wsaData;
	int res = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (res == SOCKET_ERROR) {
		logSocketErrorAndExit("WSAStartup");
	}

	// Create
	serverSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (serverSocket == INVALID_SOCKET) {
		logSocketErrorAndExit("socket");
	}

	// Add the socket to the Socket list
	sockets.push_back(serverSocket);

	// Reuse address
	int enable = 1;
	res = setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, (const char *)&enable, sizeof(int));
	if (res == SOCKET_ERROR) {
		logSocketErrorAndExit("setsockopt(SO_REUSEADDR)");
	}

	// Bind
	struct sockaddr_in serverAddress;
	serverAddress.sin_family = AF_INET;
	serverAddress.sin_port = htons(port);
	serverAddress.sin_addr.S_un.S_addr = INADDR_ANY;
	res = bind(serverSocket, (const sockaddr*)&serverAddress, sizeof(serverAddress));
	if (res == SOCKET_ERROR) {
		logSocketErrorAndExit("bind");
	}

	// Listen
	res = listen(serverSocket, 32);
	if (res == SOCKET_ERROR) {
		logSocketErrorAndExit("listen");
	}

	// Loop
	while (true)
	{
		handleIncomingData();

		// handleInput();
		// simulatePhysics();
		// simulateAI();
		// ...

		handleOutgoingData();

		// Print iteration
		static int iteration = 0;
		std::cout << "Server iteration " << iteration++ << std::endl;

		// Wait a second
		Sleep(2000);
	}

	// Delete
	closesocket(serverSocket);

	// Cleanup
	res = WSACleanup();
	if (res == SOCKET_ERROR) {
		logSocketErrorAndExit("WSACleanup");
	}
}

int main(int argc, char **argv)
{
	const int port = 8000;
	server(port);
	system("pause");
	return 0;
}
