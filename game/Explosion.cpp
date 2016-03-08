// Game Includes
#include "Explosion.h"

// Engine Includes
#include "EventStep.h"
#include "LogManager.h"
#include "GameManager.h"
#include "WorldManager.h"
#include "ResourceManager.h"

Explosion::Explosion()
{
	df::ResourceManager &resource_manager = df::ResourceManager::getInstance();

	// Setup "explosion" sprite
	df::Sprite *p_temp_sprite = resource_manager.getSprite("explosion");
	if(!p_temp_sprite) {
		df::LogManager &log_manager = df::LogManager::getInstance();
		log_manager.writeLog("Explosion::Explosion(): Warning! Sprite '%s' not found", "explosion");
		return;
	}

	setSprite(p_temp_sprite);
	setType("Explosion");

	registerInterest(df::STEP_EVENT);

	// Set live time as ling as sprite length
	time_to_live = getSprite()->getFrameCount();

	// Make explosion in foreground
	setSolidness(df::SPECTRAL);
}


void Explosion::step()
{
	time_to_live--;
	if(time_to_live <= 0)
	{
		df::WorldManager &world_manager = df::WorldManager::getInstance();
		world_manager.markForDelete(this);
	}
}

int Explosion::eventHandler(const df::Event *p_e)
{
	if(p_e->getType() == df::STEP_EVENT)
	{
		step();
		return 1;
	}

	return 0;
}