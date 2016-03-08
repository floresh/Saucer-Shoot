#include "Sentry.h"
#include "GameStart.h"


#include "EventStep.h"
#include "EventNetwork.h"
#include "WorldManager.h"
#include "NetworkManager.h"


extern pthread_mutex_t lock;

df::Sentry::Sentry()
{
	registerInterest(df::STEP_EVENT);
	setSolidness(df::SPECTRAL);
	setVisible(false);
}


void df::Sentry::doStep()
{
	df::NetworkManager &network_manager = df::NetworkManager::getInstance();

	// check socket to see if there is at least an int worth of data
	int bytes = network_manager.isData();
	if(bytes >= 4)
	{
		// sending and receiving happens here
		char buffer[4] = {0};
		int bytesReceived = network_manager.receive(buffer, 3, true);

		buffer[bytesReceived] = '\0';
		int size = atoi(buffer);

		if(bytes >= size)
		{
			// Create Network Event
			df::WorldManager &world_manager = df::WorldManager::getInstance();
			EventNetwork net;
			net.setBytes(bytes);
			world_manager.onEvent(&net);
		}
	}
}


int df::Sentry::eventHandler(const df::Event *p_e)
{
	if(p_e -> getType() == df::STEP_EVENT) {
		doStep();
		return 1;
	}

	return 0;
}