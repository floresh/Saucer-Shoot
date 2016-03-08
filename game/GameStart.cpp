// System includes
#include <iostream>
#include <sstream>
#include <string>
#include <thread>
#include <stdio.h>
#include <unistd.h>

// Game Includes
#include "Role.h"
#include "Hero.h"
#include "Saucer.h"
#include "Points.h"
#include "GameStart.h"

// Engine Includes
#include "LogManager.h"
#include "GameManager.h"
#include "EventNetwork.h"
#include "NetworkManager.h"
#include "ResourceManager.h"

#include "NetworkUtility.h"



GameStart::GameStart()
{
	Role &role = Role::getInstance();
	if(role.isHost()) {
		hasStarted = false;
	} else {
		hasStarted = true;
	}

	setType("GameStart");

	if(!role.isHost())
	{
		df::ResourceManager &resource_manager = df::ResourceManager::getInstance();
		df::Sprite *p_temp_sprite = resource_manager.getSprite("gamestart");

		if(!p_temp_sprite) {
			df::LogManager &log_manager = df::LogManager::getInstance();
			log_manager.writeLog("GameStart::GameStart(): Warning! Sprite '%s' not found", "GameStart");
		} else {
			setSprite(p_temp_sprite);
			setSpriteSlowdown(15);
		}
	}

	// Put in center of window
	setLocation(df::CENTER_CENTER);

	// Set solidness
	setSolidness(df::SPECTRAL);

	// Set altitude
	setAltitude(0);

	// Register for keyboard events
	// registerInterest(df::KEYBOARD_EVENT);
	
	// Receive network events
	registerInterest(df::NETWORK_EVENT);

	// Begin startup sequence
	start();
}




int GameStart::eventHandler(const df::Event *p_e)
{
	df::GameManager &game_manager = df::GameManager::getInstance();

	if(p_e->getType() == df::KEYBOARD_EVENT)
	{
		df::EventKeyboard *p_keyboard_event = (df::EventKeyboard *) p_e;
		switch(p_keyboard_event->getKey())
		{
			case df::Keyboard::Q:
				if(!hasStarted) {
					game_manager.setGameOver();
				}
				break;
			default:
				break;
		}
		return 1;
	}
	if(p_e->getType() == df::NETWORK_EVENT)
	{
		Role &role = Role::getInstance();
		if(!role.isHost()) {
			receiveClient();
		} else {
			receiveHost();
		}
	}

	return 0;
}


// Populate the world with some objects
void GameStart::start()
{
	Role &role = Role::getInstance();
	bool isHost = role.isHost();

	df::ViewObject *nuke = new df::ViewObject; // Count of nukes
	nuke->setLocation(df::TOP_CENTER);
	nuke->setViewString("Nukes");
	nuke->setValue(1);
	nuke->setColor(df::YELLOW);

	df::ViewObject *host = new df::ViewObject; // Count of nukes
	host->setLocation(df::TOP_LEFT);
	host->setViewString("Host");
	host->setValue(1);
	host->setColor(df::YELLOW);

	df::ViewObject *client = new df::ViewObject; // Count of nukes
	client->setLocation(df::TOP_RIGHT);
	client->setViewString("Client");
	client->setValue(1);
	client->setColor(df::YELLOW);

	if(isHost)
	{
		// receive connection from client
		df::NetworkManager &network_manager = df::NetworkManager::getInstance();
		network_manager.accept();
		hasStarted = true;

		new Hero(true);
		new Hero(false);
		// Spawn some saucers to shoot
		for(int i = 0; i < 16; i++) {
			Saucer *s1 = new Saucer;
		}

		client->setColor(df::BLUE);
	} else {
		host->setColor(df::BLUE);
	}

	// // new heads-up display
	// new Points; // points display

	// When game starts, become active
	setActive(true);
}

void GameStart::draw()
{
	// Override default draw so as not to display "value"
	// df::Object::draw();
}
