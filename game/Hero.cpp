// Game Includes
#include "Hero.h"
#include "Role.h"
#include "Bullet.h"
#include "GameOver.h"
#include "Explosion.h"
#include "EventNuke.h"

// Engine Includes
#include "ResourceManager.h"
#include "GraphicsManager.h"
#include "NetworkUtility.h"
#include "NetworkManager.h"
#include "WorldManager.h"
#include "GameManager.h"
#include "LogManager.h"
#include "Config.h"

#include "EventNetwork.h"
#include "EventMouse.h"
#include "EventStep.h"
#include "EventView.h"

// System includes
#include <iostream>

Hero::Hero(bool isHost)
{
	// Set object type
	setType("Hero");
	hostHero = isHost;

	// Link to "ship" sprite
	df::ResourceManager &resource_manager = df::ResourceManager::getInstance();
	df::LogManager &log_manager = df::LogManager::getInstance();
	Role &role = Role::getInstance();
	df::Sprite *p_temp_sprite;

	p_temp_sprite = resource_manager.getSprite("ship");
	if(!p_temp_sprite) {
		log_manager.writeLog("Hero::Hero(): Warning! Sprite '%s' not found", "ship");
	} else {

		setSprite(p_temp_sprite);
		setSpriteSlowdown(3);	// 1/3 speed animation.
		setTransparency();		// Transparent sprite
		setSolidness(df::HARD);
	}


	// Set player variables
	move_slowdown = 2;
	move_countdown = move_slowdown;

	fire_slowdown = 15;
	fire_countdown = fire_slowdown;

	nuke_count = 1;


	// Need to update rate control each step
	registerInterest(df::STEP_EVENT);

	if(role.isHost()) // This is Host code
	{
		if(isHost) // This is host player
		{
			// Create reticle for firing bullets
			p_reticle = new Reticle();
			p_reticle->draw();

			// Register for input events
			registerInterest(df::MOUSE_EVENT);
			registerInterest(df::KEYBOARD_EVENT);
			getSprite()->setColor(df::YELLOW);
		} else {
			getSprite()->setColor(df::BLUE);
		}

	} else { // This is client code
		if(!isHost) // This is client player
		{
			// Create reticle for firing bullets
			p_reticle = new Reticle();
			p_reticle->draw();

			// Register for input events
			registerInterest(df::MOUSE_EVENT);
			registerInterest(df::KEYBOARD_EVENT);
			getSprite()->setColor(df::YELLOW);
		} else {
			getSprite()->setColor(df::BLUE);
		}
	}


	// Set starting location
	if(isHost)
	{
		df::WorldManager &world_manager = df::WorldManager::getInstance();
		df::Position pos(7, world_manager.getBoundary().getVertical()/2 + 7);
		setPosition(pos);

	} else {
		df::WorldManager &world_manager = df::WorldManager::getInstance();
		df::Position pos(7, world_manager.getBoundary().getVertical()/2 - 7);
		setPosition(pos);
	}

	if(role.isHost()) {
		send_event(this, CREATE); // Send object creation event to client
	}
}


Hero::~Hero()
{
	// Create GameOver object
	GameOver *p_go = new GameOver;

	// Make big explosion
	for(int i = -8; i <= 8; i += 5)
	{
		for(int j = -5; j <= 5; j += 3)
		{
			df::Position temp_pos = this->getPosition();

			temp_pos.setX(this->getPosition().getX() + i);
			temp_pos.setY(this->getPosition().getY() + j);

			Explosion *p_explosion = new Explosion;
			p_explosion -> setPosition(temp_pos);
		}
	}

	// Mark Reticle for deletion
	df::WorldManager::getInstance().markForDelete(p_reticle);

	// Send deletion event to client
	send_event(this, DELETE);
}

// Move up or down
void Hero::move(int dy)
{
	if(move_countdown > 0) {
		return;
	}
	move_countdown = move_slowdown;

	//If stays on window, allow move
	df::Position new_pos(getPosition().getX(), getPosition().getY() + dy);
	df::WorldManager &world_manager = df::WorldManager::getInstance();

	if((new_pos.getY() > 3) && 
		(new_pos.getY() < world_manager.getBoundary().getVertical()-1)) 
	{
		world_manager.moveObject(this, new_pos);

		Role &role = Role::getInstance();
		if(role.isHost()) {
			send_event(this, UPDATE);
		}
	}
}

void Hero::step()
{
	// Move countdown
	move_countdown--;
	fire_countdown--;
	if(move_countdown < 0) {
		move_countdown = 0;
	}
	if(fire_countdown < 0) {
		fire_countdown = 0;
	}
}


void Hero::fire(df::Position target, bool hostBullet)
{
	std::string x = std::to_string(target.getX());
	std::string y = std::to_string(target.getY());
	std::cout << "x:" << x << "\ny:" << y << "\n";

	if(fire_countdown > 0) {
		return;
	}
	fire_countdown = fire_slowdown;

	// Fire Bullet towards target
	Bullet *p = new Bullet(getPosition(), hostBullet);
	p->setYVelocity((float) (target.getY() - getPosition().getY()) /
					(float) (target.getX() - getPosition().getX()));
}


void Hero::mouse(const df::EventMouse *p_mouse_event)
{
	// If left mouse button was clicked
	if( (p_mouse_event->getMouseAction() == df::CLICKED) &&
		(p_mouse_event->getMouseButton() == df::Mouse::LEFT))
	{
		// If mouse was click inside of window
		sf::RenderWindow *p_win = df::GraphicsManager::getInstance().getWindow();
		sf::Vector2i lp = sf::Mouse::getPosition(*p_win);
		if (lp.x > df::Config::getInstance().getWindowHorizontalPixels() ||       
		    lp.x < 0 ||
		    lp.y > df::Config::getInstance().getWindowVerticalPixels() ||
		    lp.y < 0)
		{
		  // Outside window so don't capture input.
		} else {
			// Inside window so don't capture input.
			df::Position mouse_pos = p_mouse_event->getMousePosition();
			send_mouse(mouse_pos.getX(), mouse_pos.getY(), this->getId());
		}

		fire(p_mouse_event->getMousePosition(), true);
	}
}

int Hero::eventHandler(const df::Event *p_e)
{
	if(p_e->getType() == df::KEYBOARD_EVENT) {
		const df::EventKeyboard *p_keyboard_event = dynamic_cast <const df::EventKeyboard *> (p_e);
		kbd(p_keyboard_event);
		return 1;
	}

	if(p_e->getType() == df::MOUSE_EVENT) {
		const df::EventMouse *p_mouse_event = dynamic_cast <const df::EventMouse *> (p_e);
		mouse(p_mouse_event);
		return 1;
	}

	if(p_e->getType() == df::STEP_EVENT) {
		step();
		return 1;
	}

	return 0;
}



//Take appropriate action according to key pressed
void Hero::kbd(const df::EventKeyboard *p_keyboard_event)
{
	Role &role = Role::getInstance();
	if(role.isHost())
	{
		switch(p_keyboard_event->getKey())
		{
			case df::Keyboard::Q:
				if(p_keyboard_event->getKeyboardAction() == df::KEY_PRESSED) {
					df::WorldManager &world_manager = df::WorldManager::getInstance();
					world_manager.markForDelete(this);
				}
				break;
			case df::Keyboard::W:
				if(p_keyboard_event->getKeyboardAction() == df::KEY_DOWN) {
					move(-1);
				}
				break;
			case df::Keyboard::S:
				if(p_keyboard_event->getKeyboardAction() == df::KEY_DOWN) {
					move(+1);
				}
				break;
			case df::Keyboard::SPACE:
				if(p_keyboard_event->getKeyboardAction() == df::KEY_DOWN) {
					if(this->hostHero) {
						fputs("Host player is nuking!\n", stdout);
						nuke();
					} 
				}
			case df::Keyboard::N:
				if(p_keyboard_event->getKeyboardAction() == df::KEY_DOWN) {
					if(!this->hostHero) {
						fputs("Client player is nuking!\n", stdout);
						nuke();
					} 
				}
		}
	} else {

		switch(p_keyboard_event->getKey())
		{
			// case df::Keyboard::Q:
				if(p_keyboard_event->getKeyboardAction() == df::KEY_PRESSED) {
					df::WorldManager &world_manager = df::WorldManager::getInstance();
					world_manager.markForDelete(this);
					send_key('Q', getId());
					fputs("Sent the Q press over", stdout);
				}
				break;
			case df::Keyboard::UPARROW:
				if(p_keyboard_event->getKeyboardAction() == df::KEY_DOWN) {
					move(-1);
					send_key('W', getId());
				}
				break;
			case df::Keyboard::DOWNARROW:
				if(p_keyboard_event->getKeyboardAction() == df::KEY_DOWN) {
					move(+1);
					send_key('S', getId());
				}
				break;
			case df::Keyboard::N:
				if(p_keyboard_event->getKeyboardAction() == df::KEY_DOWN) {
					// if(!hostHero) {
						fputs("Client is nuking!", stdout);
						send_key('N', getId());
						nuke();
					// }
				}
		}
	}
}


// Handle player nuking
void Hero::nuke()
{
	// Check if nukes left
	if(!nuke_count) {
		return;
	}
	nuke_count--;

	// Create "nuke" event and send to interested Objects
	df::WorldManager &world_manager = df::WorldManager::getInstance();
	EventNuke nuke;
	world_manager.onEvent(&nuke);

	// Send "view" event with nukes to interested ViewObjects
	Role &role = Role::getInstance();
	if(role.isHost()) {
		if(hostHero) {
			df::EventView ev("Nukes", -1, true);
			world_manager.onEvent(&ev);
		}
	} else {
		if(!hostHero) {
			df::EventView ev("Nukes", -1, true);
			world_manager.onEvent(&ev);
		}
	}
}