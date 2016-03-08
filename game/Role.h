//
// Role class
//
// Indicate whether game is Host or Client
//

#pragma once

class Role
{
private:
	Role();						 // Private since a singleton
	Role(Role const&);			 // Don't allow copy
	void operator=(Role const&); // Don't allow assignment
	bool is_host;				 // True if hosting game

public:
	// Get the one and only instance of the Role
	static Role &getInstance();

	// Set host
	void setHost(bool is_host = true);

	// Return true if host
	bool isHost() const;
};