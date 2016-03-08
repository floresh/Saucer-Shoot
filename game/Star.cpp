// System includes
#include <stdlib.h>

// Game Includes
#include "Star.h"

// Engine Includes
#include "EventOut.h"
#include "WorldManager.h"
#include "GraphicsManager.h"

Star::Star()
{
	setType("Star");
	setSolidness(df::SPECTRAL);

	setXVelocity((float) (-1.0 / (rand()%10 + 1)));
	setAltitude(0); // Make stars in background

	df::WorldManager &world_manager = df::WorldManager::getInstance();
	df::Position pos(rand() % world_manager.getBoundary().getHorizontal(),
					 rand() % world_manager.getBoundary().getVertical());
	setPosition(pos);
}


void Star::draw()
{
	df::GraphicsManager &graphics_manager = df::GraphicsManager::getInstance();
	graphics_manager.drawCh(getPosition(), STAR_CHAR, df::WHITE);
}


void Star::out()
{
	df::WorldManager &world_manager = df::WorldManager::getInstance();
	df::Position pos(world_manager.getBoundary().getHorizontal() + rand()%20,
					 rand() % world_manager.getBoundary().getVertical());
	setPosition(pos);
	setXVelocity((float) (-1.0 / (rand()%10 + 1)));
}



int Star::eventHandler(const df::Event *p_e)
{
	if(p_e->getType() == df::OUT_EVENT) {
		out();
		return 1;
	}

	return 0;
}