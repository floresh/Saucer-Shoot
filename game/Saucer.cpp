// System Includes
#include <stdlib.h> // For rand()

// Game Includes
#include "Role.h"
#include "Saucer.h"
#include "Points.h"
#include "Bullet.h"
#include "Explosion.h"
#include "EventNuke.h"

// Engine Includes
#include "EventOut.h"
#include "EventView.h"
#include "EventCollision.h"
#include "LogManager.h"
#include "WorldManager.h"
#include "ResourceManager.h"
#include "NetworkUtility.h"

extern int host_score;
extern int client_score;

Saucer::Saucer()
{
	// Dragonfly managers needed for this method
	df::LogManager &log_manager = df::LogManager::getInstance();
	df::WorldManager &world_manager = df::WorldManager::getInstance();
	df::ResourceManager &resource_manager = df::ResourceManager::getInstance();

	// Setup "saucer" sprite
	df::Sprite *p_temp_sprite = resource_manager.getSprite("saucer");
	if(!p_temp_sprite) {
		log_manager.writeLog("Saucer::Saucer(): Warning! Sprite '%s' not found", "saucer");
	} else {
		setSprite(p_temp_sprite);
		setSpriteSlowdown(4);
	}

	// Register for nuke
	registerInterest(NUKE_EVENT);

	// Set object type
	setType("Saucer");

	// Set speed in horizontal direction
	setXVelocity(-0.25); // 1 space left every 4 frames

	// move to start position of right side of frame
	moveToStart();

	Role &role = Role::getInstance();
	if(role.isHost()) {
		// Send object creation event to client
		send_event(this, CREATE);
	}
}

Saucer::~Saucer()
{
	// Send view events with points to interested ViewObjects
	df::EventView ev(POINTS_STRING, 10, true); // Add 10 points

	Role &role = Role::getInstance();
	if(role.isHost()) {
		send_event(this, DELETE);
	} else {
		Explosion *p_explosion = new Explosion;
		p_explosion -> setPosition(this -> getPosition());
	}

	df::WorldManager &world_manager = df::WorldManager::getInstance();
	world_manager.onEvent(&ev);
}

void Saucer::moveToStart()
{
	df::WorldManager &world_manager = df::WorldManager::getInstance();
	df::Position temp_pos;

	int world_horiz = world_manager.getBoundary().getHorizontal();
	int world_vert = world_manager.getBoundary().getVertical();

	// x is off right side of window
	temp_pos.setX(world_horiz + rand()%world_horiz + 3);
	temp_pos.setY(rand()%(world_vert-1) + 4);

	world_manager.moveObject(this, temp_pos);

	// If collision, move right slightly until empty space
	df::ObjectList collision_list = world_manager.isCollision(this, temp_pos);
	while(!collision_list.isEmpty())
	{
		temp_pos.setX(temp_pos.getX() + 1);
		collision_list = world_manager.isCollision(this, temp_pos);
	}

	world_manager.moveObject(this, temp_pos);
}

void Saucer::out()
{
	if(getPosition().getX() >= 0) {
		return;
	}

	moveToStart();

	// Spawn nre saucer to make the game get harder
	new Saucer;
}


void Saucer::hit(const df::EventCollision *p_c)
{
	// If Saucer on Saucer, ignore.
	if( (p_c -> getObject1() -> getType() == "Saucer") &&
		(p_c -> getObject2() -> getType() == "Saucer")) {
		return;
	}
	// If bullet...
	if( ((p_c -> getObject1() -> getType()) == "Bullet") ||
		((p_c -> getObject1() -> getType()) == "Bullet"))
	{
		// Create an explosion
		Explosion *p_explosion = new Explosion;
		p_explosion -> setPosition(this -> getPosition());



		bool hostBullet;
		std::string o1 = p_c -> getObject1() -> getType();
		df::WorldManager &world_manager = df::WorldManager::getInstance();

		if(o1 == "Bullet") {

			int id = p_c->getObject1()->getId();
			Bullet *b = (Bullet*)(world_manager.objectWithId(id));

			hostBullet = b->isHostBullet();
		
		} else {
		
			df::WorldManager &world_manager = df::WorldManager::getInstance();
			int id = p_c->getObject2()->getId();
			Bullet *b = (Bullet*)(world_manager.objectWithId(id));

			hostBullet = b->isHostBullet();
		}

		if(hostBullet) {
			df::EventView ev("Host", 1, true);
			world_manager.onEvent(&ev);
			host_score++;
		} else {
			df::EventView ev("Client", 1, true);
			world_manager.onEvent(&ev);
			client_score++;
		}

		send_score(host_score, client_score);

		// Saucers appear to stay around perpetually
		new Saucer;
	}
	// If Hero, mark both objects for destruction
	if( (p_c -> getObject1() -> getType() == "Hero") ||
		(p_c -> getObject2() -> getType() == "Hero"))
	{
		fputs("Host is deleting Hero\n", stdout);
		df::WorldManager &world_manager = df::WorldManager::getInstance();
		world_manager.markForDelete(p_c -> getObject1());
		world_manager.markForDelete(p_c -> getObject2());
	}
}


int Saucer::eventHandler(const df::Event *p_e)
{
	Role &role = Role::getInstance();
	if(role.isHost())
	{
		if(p_e->getType() == df::OUT_EVENT)
		{
			out();
			return 1;
		}
		if(p_e->getType() == df::COLLISION_EVENT)
		{
			const df::EventCollision *p_collision_event = dynamic_cast <df::EventCollision const *> (p_e);
			hit(p_collision_event);
			return 1;
		}
		if(p_e->getType() == NUKE_EVENT)
		{
			Explosion *p_explosion = new Explosion;
			p_explosion -> setPosition(this -> getPosition());

			df::WorldManager &world_manager = df::WorldManager::getInstance();
			world_manager.markForDelete(this);

			new Saucer;
		}
	}

	return 0;
}

