// System includes
#include <string.h>

// Game includes
#include "Star.h"
#include "Role.h"
#include "Sentry.h"
#include "GameStart.h"

// Engine includes
#include "Pause.h"
#include "Color.h"
#include "LogManager.h"
#include "GameManager.h"
#include "NetworkManager.h"
#include "ResourceManager.h"

int host_score = 0;
int client_score = 0;

void loadResources(void);
void populateWorld(void);

int main(int argc, char *argv[])
{
	char *host;
	Role &role = Role::getInstance();

	if(argc == 2 && strcmp(argv[1], "-h") == 0) {
		role.setHost(true);
	}
	else if(argc == 3 && strcmp(argv[1], "-c") == 0) {
		role.setHost(false);
		host = argv[2];
	}
	else {
		fputs("Usage: ./game [-h | -c <host_address>]\n", stderr);
		return -1;
	}


	df::LogManager &log_manager = df::LogManager::getInstance();
	df::GameManager &game_manager = df::GameManager::getInstance();
	df::NetworkManager &network_manager = df::NetworkManager::getInstance();

	// Start up Game Manager
	if(game_manager.startUp())
	{
		log_manager.writeLog("Error starting game manager");
		game_manager.shutDown();
		return 0;
	}

	// Set flush of logfile during development (when done, make false).
	log_manager.setFlush(true);

	// Show splash screen
	df::splash();

	// if host: wait for connection
	if(!role.isHost()) {
		// if client: connect to host
		std::string s(host);
		if( network_manager.connect(s) < 0 ) {
			fputs("ERROR connect()\n", stderr);
			return -1;
		}
	}

	// Load game Resources
	loadResources();
	
	//Populate game world with some objects
	populateWorld();

	// Run game (this blocks until the game loop is over)
	fputs("starting game manager\n", stdout);
	game_manager.run();

	// Shut everything down.
	game_manager.shutDown();
}

void loadResources(void)
{
	// Load saucer sprite
	df::ResourceManager &resource_manager = df::ResourceManager::getInstance();
	resource_manager.loadSprite("sprites/saucer-spr.txt", "saucer");
	resource_manager.loadSprite("sprites/ship-spr.txt", "ship");
	resource_manager.loadSprite("sprites/bullet-spr.txt", "bullet");
	resource_manager.loadSprite("sprites/explosion-spr.txt", "explosion");
	resource_manager.loadSprite("sprites/gameover-spr.txt", "gameover");
	resource_manager.loadSprite("sprites/gamestart-spr.txt", "gamestart");
}

void populateWorld(void)
{	
	// Create some stars
	for(int i=0; i < 16; i++) {
		new Star;
	}

	// Spawn GameStart object
	new df::Sentry();
	new GameStart();
}