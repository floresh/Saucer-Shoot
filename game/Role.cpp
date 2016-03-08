#include "Role.h"

Role::Role() {}

// Get the only instance of Role
Role &Role::getInstance()
{
	static Role singleton;
	return singleton;
}

//set host
void Role::setHost(bool is_host)
{
	Role::is_host = is_host;
}

// Return true if host
bool Role::isHost() const
{
	return Role::is_host;
}