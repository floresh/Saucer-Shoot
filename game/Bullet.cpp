// Game Includes
#include "Bullet.h"
#include "Saucer.h"

// Engine Includes
#include "EventOut.h"
#include "LogManager.h"
#include "WorldManager.h"
#include "ResourceManager.h"


Bullet::Bullet(df::Position hero_pos, bool isHost)
{
	hostBullet = isHost;
	
	// Dragonfly managers
	df::LogManager &log_manager = df::LogManager::getInstance();
	df::WorldManager &world_manager = df::WorldManager::getInstance();
	df::ResourceManager &resource_manager = df::ResourceManager::getInstance();

	// Setup "bullet" sprite
	df::Sprite *p_temp_sprite = resource_manager.getSprite("bullet");
	if(!p_temp_sprite) {
		log_manager.writeLog("Bullet::Bullet(): Warning! Sprite %s not found!", "bullet");
	} else {
		setSprite(p_temp_sprite);
		setSpriteSlowdown(5);
	}


	// Set object type
	setType("Bullet");

	// Set speed in horizontal direction
	setXVelocity(1);

	// Set starting location, based on hero's position passed in
	df::Position pos(hero_pos.getX()+3, hero_pos.getY());
	setPosition(pos);

}


// If bullet hits saucer, mark saucer and bullet for deletion
void Bullet::hit(const df::EventCollision *p_c)
{
	if( (p_c -> getObject1() -> getType() == "Bullet") &&
		(p_c -> getObject2() -> getType() == "Bullet"))
	{
		return;
	}
	
	df::WorldManager &world_manager = df::WorldManager::getInstance();
	world_manager.markForDelete(p_c->getObject1());
	world_manager.markForDelete(p_c->getObject2());
}


// If bullet moves outside world, mark self for deletion
void Bullet::out()
{
	df::WorldManager &world_manager = df::WorldManager::getInstance();
	world_manager.markForDelete(this);
}


int Bullet::eventHandler(const df::Event *p_e)
{
	if(p_e->getType() == df::OUT_EVENT) 
	{
		out();
		return 1;
	}
	if(p_e->getType() == df::COLLISION_EVENT) 
	{
		const df::EventCollision *p_collision_event = dynamic_cast <const df::EventCollision *> (p_e);
		hit(p_collision_event);
		return 1;
	}

	return 0;
}



bool Bullet::isHostBullet() {
	return hostBullet;
}

