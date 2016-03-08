#include "EventNetwork.h"

using namespace df;

// Default constructor
EventNetwork::EventNetwork()
{
	setType(NETWORK_EVENT);
}

// Create object with initial bytes
EventNetwork::EventNetwork(int initial_bytes)
{
	EventNetwork::bytes = initial_bytes;
}

// Set number of bytes available
void EventNetwork::setBytes(int new_bytes)
{
	EventNetwork::bytes = new_bytes;
}

// Get number of bytes available
int EventNetwork::getBytes() const
{
	return 0;
}