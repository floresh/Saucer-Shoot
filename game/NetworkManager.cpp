// System includes
#include <netdb.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>

#include <iostream>

// Game includes
#include "NetworkManager.h"
#include "EventNetwork.h"

using namespace df;

NetworkManager::NetworkManager(){}

NetworkManager &NetworkManager::getInstance()
{
	static NetworkManager singleton;
	return singleton;
}

// Start up NetworkManager
int NetworkManager::startUp()
{
	sock = -1;

	return 0;
}

// Shut down NetworkManager
void NetworkManager::shutDown()
{
	if(sock > 0) {
		close();
		sock = -1;
	}
}


// Accept only network events
// Returns false for other engine events
bool NetworkManager::isValid(std::string event_type) const
{
	return event_type == NETWORK_EVENT;
}


// Block, waiting to accept network connection
int NetworkManager::accept(std::string port /*= DRAGONFLY_PORT*/)
{
	int port_int = std::stoi(port); // convert string to int
	
	//////////////////////////////////////////////////////////////////////////////////
	// Create an unconnected socket to which a remote client can connect => socket()//
	//////////////////////////////////////////////////////////////////////////////////
	int servSock = socket(AF_INET, SOCK_STREAM, 0);
	if(servSock < 0)
	{
		perror("ERROR socket()");
		exit(1);
	}

	int on = 1;
	if(setsockopt(servSock, SOL_SOCKET, SO_REUSEADDR, &on, sizeof on) < 0) {
		perror("ERROR setsockopt()");
		exit(1);
	}


	//////////////////////////////////////////////////////////////////////////////////
	// Bind to the server's local address and port so a client can connect => bind()//
	//////////////////////////////////////////////////////////////////////////////////

	struct sockaddr_in servAddr;
	bzero((char *) &servAddr, sizeof(servAddr));
	servAddr.sin_family = AF_INET;
	servAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servAddr.sin_port = htons((unsigned short)port_int);

	if(bind(servSock, (struct sockaddr *)&servAddr, sizeof(servAddr)) < 0) {
		perror("ERROR bind()");
		exit(1);
	}

	// Print local address of socket
	struct sockaddr_storage localAddr;
	socklen_t addrSize = sizeof(localAddr);


	if (getsockname(servSock, (struct sockaddr *) &localAddr, &addrSize) < 0) {
		perror("ERROR getsockname() failed");
		exit(5);
	}


	fputs("Binding to ", stdout);
	PrintSocketAddress((struct sockaddr *) &localAddr, stdout);
	fputc('\n', stdout);


	///////////////////////////////////////////////////////////////////////////////////////////////
	// Put the socket in a state to accept the other "half" of an internet connection => listen()//
	///////////////////////////////////////////////////////////////////////////////////////////////
	listen(servSock, 5);

	///////////////////////////////////////////////////////////
	// Wait (block) until the socket is connected => accept()//
	///////////////////////////////////////////////////////////

	struct sockaddr_storage clntAddr; // Client address

	// Set length of client address structure (in-out parameter)
	socklen_t clntAddrLen = sizeof(clntAddr);

	// Wait for a client to connect
	sock = ::accept(servSock, (struct sockaddr *) &clntAddr, &clntAddrLen);
	if (sock < 0) {
		perror("ERROR accept()");
		exit(2);
	}

	
	return 0;
}




// Make network connection
// Return 0 if success, else -1
int NetworkManager::connect(std::string host, std::string port /*= DRAGONFLY_PORT*/)
{
	std::cout << "Connecting to port: " << port << "\n";
	////////////////////////////////////////////////////////////
	// Lookup the host name, translating it into the necessary//
	// system structure for connections => getaddrinfo()	  //
	// Tell the system what kind(s) of address info we want	  //
	////////////////////////////////////////////////////////////
	struct addrinfo addrCriteria;                   // Criteria for address match
	memset(&addrCriteria, 0, sizeof(addrCriteria)); // Zero out structure
	addrCriteria.ai_family = AF_UNSPEC;             // v4 or v6 is OK
	addrCriteria.ai_socktype = SOCK_STREAM;         // Only streaming sockets
	addrCriteria.ai_protocol = IPPROTO_TCP;         // Only TCP protocol

	// Get address(es)
	struct addrinfo *servAddr; // Holder for returned list of server addrs
	int rtnVal = getaddrinfo(host.c_str(), port.c_str(), &addrCriteria, &servAddr);
	if (rtnVal != 0) {
		printf("ERROR getaddrinfo() failed: %s\n", gai_strerror(rtnVal));
	}



	////////////////////////////////////////////////////////////////////////////
	// Create a socket used by the client to connect to the server => socket()//
	// Connect to the server => connect()									  //
	////////////////////////////////////////////////////////////////////////////
	sock = -1;
	for (struct addrinfo *addr = servAddr; addr != NULL; addr = addr->ai_next)
	{
		// Create a reliable, stream socket using TCP
		sock = socket(addr->ai_family, addr->ai_socktype, addr->ai_protocol);
		if (sock < 0) {
			continue;  // Socket creation failed; try next address
		}

		// Establish the connection to the echo server
		if (::connect(sock, addr->ai_addr, addr->ai_addrlen) == 0) {
			// freeaddrinfo(servAddr); // Free addrinfo allocated in getaddrinfo()

			return 0;     // Socket connection succeeded;
		}

		::close(sock); // Socket connection failed; try next address
		sock = -1;
	}

	freeaddrinfo(servAddr); // Free addrinfo allocated in getaddrinfo()
	
	return -1;
}


// Close network connection
// Return 0 if success, else -1
int NetworkManager::close()
{
	return ::close(sock);
}


// Return true if network connected, else false
bool NetworkManager::isConnected() const
{
	return sock > 0;
}

// Return socket
int NetworkManager::getSocket() const
{
	return sock;
}


// Sent buffer to connected network
// Return 0 if success, else -1
int NetworkManager::send(void *buffer, int bytes)
{
	ssize_t bytesSent = ::send(sock, buffer, bytes, 0);

	if(bytesSent < 0) {
		perror("ERROR send()");
		return -1;
	}

	return 0;
}


// Receive from connected 	network (no more than nbytes)
// If peek is true, leave data in socket, else remove
// Return number of bytes reveived, else -1 if error
int NetworkManager::receive(void *buffer, int nbytes, bool peek /*= false*/)
{
	// recv() -> non-blocking
	if(peek) {
		return recv(sock, buffer, nbytes, MSG_PEEK);
	}

	return recv(sock, buffer, nbytes, MSG_WAITALL);
}


// Check if network data
// Return amount of data (0 if no data), -1 if not connected or error
int NetworkManager::isData() const
{
	// uint64_t data;
	// int bytesRead = recv(sock, &data, sizeof data, MSG_PEEK);
	int count;
	ioctl(sock, FIONREAD, &count);

	return count;
}















void NetworkManager::PrintSocketAddress(const struct sockaddr *address, FILE *stream) {
  // Test for address and stream
  if (address == NULL || stream == NULL)
    return;

  void *numericAddress; // Pointer to binary address
  // Buffer to contain result (IPv6 sufficient to hold IPv4)
  char addrBuffer[INET6_ADDRSTRLEN];
  in_port_t port; // Port to print
  // Set pointer to address based on address family
  switch (address->sa_family) {
  case AF_INET:
    numericAddress = &((struct sockaddr_in *) address)->sin_addr;
    port = ntohs(((struct sockaddr_in *) address)->sin_port);
    break;
  case AF_INET6:
    numericAddress = &((struct sockaddr_in6 *) address)->sin6_addr;
    port = ntohs(((struct sockaddr_in6 *) address)->sin6_port);
    break;
  default:
    fputs("[unknown type]", stream);    // Unhandled type
    return;
  }
  // Convert binary to printable address
  if (inet_ntop(address->sa_family, numericAddress, addrBuffer,
      sizeof(addrBuffer)) == NULL)
    fputs("[invalid address]", stream); // Unable to convert
  else {
    fprintf(stream, "%s", addrBuffer);
    if (port != 0)                // Zero not valid in any socket addr
      fprintf(stream, "-%u", port);
  }
}