//
// A "network" event, generated when a network packet arrives
//

#pragma once

#include "Event.h"

namespace df
{
	const std::string NETWORK_EVENT = "df::network";

	class EventNetwork : public Event
	{
	private:
		int bytes; // Number of bytes available

	public:
		// Default constructor
		EventNetwork();

		// Create object with initial bytes
		EventNetwork(int initial_bytes);

		// Set number of bytes available
		void setBytes(int new_bytes);

		// Get number of bytes available
		int getBytes() const;
	};
} // end of namespace df