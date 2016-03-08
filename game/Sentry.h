//
// Sentry
//
// Poll NetworkManager for incoming messages, generating network
// events (onEvent()) when there complete messages available

#pragma once

#include "Object.h"
#define BUFFLEN 1024

namespace df
{
	class Sentry : public Object
	{
	private:
		void doStep();

	public:
		Sentry();
		int eventHandler(const Event *p_e);
	};
} // end of namespace df