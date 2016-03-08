// Game Includes
#include "GameOver.h"
#include "Explosion.h"

// Engine Includes
#include "EventStep.h"
#include "LogManager.h"
#include "GameManager.h"
#include "WorldManager.h"
#include "ResourceManager.h"

GameOver::GameOver()
{
	setType("GameOver");

	df::ResourceManager &resource_manager = df::ResourceManager::getInstance();
	df::Sprite *p_temp_sprite = resource_manager.getSprite("gameover");

	if(!p_temp_sprite) {
		df::LogManager &log_manager = df::LogManager::getInstance();
		log_manager.writeLog("GameOver::GameOver(): Warning! Sprite '%s' not found", "gameover");
	} else {
		setSprite(p_temp_sprite);
		setSpriteSlowdown(15);
		setTransparency('#'); // Transparent Character
		time_to_live = p_temp_sprite->getFrameCount() * 15;
	}

	// put in center of window
	setLocation(df::CENTER_CENTER);

	// Register for step event
	registerInterest(df::STEP_EVENT);

}


GameOver::~GameOver()
{
	df::WorldManager &world_manager = df::WorldManager::getInstance();

	// Remove Saucers and ViewObjects, re-activate GameStart
	df::ObjectList object_list = world_manager.getAllObjects(true);
	df::ObjectListIterator i(&object_list);

	for(i.first(); !i.isDone(); i.next())
	{
		df::Object *p_o = i.currentObject();
		if(p_o -> getType() == "Saucer" || p_o -> getType() == "ViewObject") {
			world_manager.markForDelete(p_o);
		}
		if(p_o -> getType() == "GameStart") {
			p_o -> setActive(true);
		}
	}

	df::GameManager &game_manager = df::GameManager::getInstance();
	game_manager.setGameOver();
}


int GameOver::eventHandler(const df::Event *p_e)
{
	if(p_e -> getType() == df::STEP_EVENT)
	{
		step();
		return 1;
	}

	return 0;
}


void GameOver::step()
{
	time_to_live--;
	if(time_to_live <= 0)
	{
		df::WorldManager &world_manager = df::WorldManager::getInstance();
		world_manager.markForDelete(this);
	}
}

void GameOver::draw()
{
	// Override default draw so as not to display "value"
	df::Object::draw();
}