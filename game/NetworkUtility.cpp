#include "Role.h"
#include "Hero.h"
#include "Saucer.h"
#include "Bullet.h"

#include "Object.h"
#include "utility.h"
#include "EventView.h"
#include "ObjectList.h"
#include "GameManager.h"
#include "WorldManager.h"
#include "ResourceManager.h"
#include "NetworkUtility.h"
#include "NetworkManager.h"

#include "EventKeyboard.h"

#include <iostream>
#include <sstream>
#include <string>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>




/*
 * The folowing function are to be used by the Host to:
 *		+ Receive keyboard and mouse events from the client
 *		+ Send object creations, deletions, and updates
 *		+ Send score updates to the client
 *
 */


/* send_event(Object obj, std::string event)
 *	Sends serialized object to client
 *	Complete list of object attributes:
 *		+ ObjectID
 *		+ Object Position(int x, int y)
 */
void send_event(df::Object *obj, std::string event)
{
	std::string ss;
	ss += "event:" + event + ",";
	ss += obj->serialize(true);

	package_message(ss);
}


/*
 * send_score(int host, int client)
 *	sends host's updates of client
 *	and host scores.
 */
void send_score(int host, int client)
{
	std::string ss;
	ss += "event:" + SCORE + ",";
	ss += "host:" + std::to_string(host) + ",";
	ss += "client:" + std::to_string(client) + ";";

	std::cout << ss << "\n";

	package_message(ss);	
}



/*
 * receiveHost(void);
 *	to be called inside of a network event handler on the host.
 *	Parses message to extract event type and dispatch correct
 *	handler function.
 *
 *	If error return -1, else return 0
 */
int receiveHost()
{
	df::NetworkManager &network_manager = df::NetworkManager::getInstance();

	// Get size of message
	char size_string[4] = {0};
	int bytes = network_manager.receive(size_string, 3, false);

	// If receive() failed: Log the error and return -1
	if(bytes <= 0)  {
		fprintf(stderr, "Receive returned: [%d]\n", bytes);
		return -1;
	}

	size_string[bytes] = '\0';
	int size = atoi(size_string);


	// Read in serialized message to buffer
	char buffer[size+1];
	size_t len = size;
	bytes = network_manager.receive(buffer, len, false);

	// If receive() failed: Log the error and return -1
	if(bytes <= 0 || bytes != size) {
		fprintf(stderr, "Receive returned: [%d]\n", bytes);
		return -1;
	}

	// Turn message buffer into string and extract event type
	std::string msg(buffer);
	int event = std::stoi(df::match(msg, "event"));

	// Dispatch event handler
	switch(event)
	{
		case 3: // MOUSE
			rec_mouse(msg);
			break;

		case 4: // KEYBOARD
			rec_key(msg);
			break;
		
		default:
			break;
	}

	return 0;
}


/*
 * rec_mouse(std::string msg)
 *	mouse event handler. Uses mouse x and y and player id to handle mouse clicks.
 *	This function works on both client and host.
 */
void rec_mouse(std::string msg)
{
	Role &role = Role::getInstance();
	bool isHost = role.isHost();

	int x = stoi(df::match(msg, "mouse_x"));
	int y = stoi(df::match(msg, "mouse_y"));
	int id = stoi(df::match(msg, "id"));
	id = (isHost) ? id - ID_OFFSET : id + ID_OFFSET;


	df::WorldManager &world_manager = df::WorldManager::getInstance();
	Hero *hero = (Hero*)world_manager.objectWithId(id);
	
	df::Position target(x,y);
	hero->fire(target, false);
}


/*
 * rec_key(std::string msg)
 *	This function receives a single keyboard event and player id
 *	and send the keyboard event to the host's instance of that player.
 */
void rec_key(std::string msg)
{
	// Extract keyboard event and player id
	int id = stoi(df::match(msg, "id")) - ID_OFFSET;
	char key = df::match(msg, "key").c_str()[0];

	// Create DragonFly keyboard event
	df::Keyboard::Key df_key;
	df::EventKeyboardAction action = df::KEY_DOWN;
	switch(key) 
	{
		case 'Q': df_key = df::Keyboard::Q; 
				  action = df::KEY_PRESSED; 
				  fputs("Got the Q press\n", stdout);
				  break;
		case 'W': df_key = df::Keyboard::W; break;
		case 'S': df_key = df::Keyboard::S; break;
		case 'N': df_key = df::Keyboard::N; break;
		default:  df_key = df::Keyboard::L; break;
	}

	df::EventKeyboard *ek = new df::EventKeyboard;
	ek->setKey(df_key);
	ek->setKeyboardAction(action);

	// Get pointer to Hero using player's ID
	df::WorldManager &world_manager = df::WorldManager::getInstance();
	Hero *hero = (Hero*)world_manager.objectWithId(id);
	if(hero == NULL) 
	{
		// If player ID not found: Log event and return;
		fputs("rec_key(): hero is NULL", stdout);
		return;
	}

	// Send keyboard event to hero
	hero->kbd(ek);
}






















/*
 * The following functions are to be used by the client to:
 *		+ Receive object creations, deletions, and updates
 *		+ Receive updates of the host player's score
 *		+ Send keyboard and mouse events to the host
 */

int receiveClient()
{
	df::NetworkManager &network_manager = df::NetworkManager::getInstance();

	// Get size of message
	char size_string[4] = {0};
	int bytes = network_manager.receive(size_string, 3, false);
	// If receive() failed: Log event and return -1
	if(bytes <= 0) {
		fprintf(stderr, "Receive returned: [%d]\n", bytes);
		return -1;
	}

	// Convert size string to int
	size_string[bytes] = '\0';
	int size = atoi(size_string);


	// Read in serialized message to buffer
	char buffer[size+1];
	size_t len = size;
	bytes = network_manager.receive(buffer, len, false);

	// If receive() failed: Log event and return -1
	if(bytes <= 0) {
		fprintf(stderr, "Receive returned: [%d]\n", bytes);
		return -1;
	}

	buffer[bytes] = '\0';
	std::string msg(buffer);

	// Dispatch event handler
	int event = std::stoi(df::match(msg, "event"));
	switch(event)
	{
		case 0: // CREATE
			rec_create(msg);
			break;
		case 1: // UPDATE
			rec_update(msg);
			break;
		case 2: // DELETE
			rec_delete(msg);
			break;
		case 3: // MOUSE
			rec_mouse(msg);
			break;
		case 5: // SCORE
			rec_score(msg);
			break;
		default:
			return 0;
			break;
	}

	return 0;
}



/*
 * rec_score(std::string msg)
 *	Extracts host+client score updates from message and
 *	sends thoe updates to host/client EventView
 */
void rec_score(std::string msg)
{
	// Extract host score and client score
	int host = stoi(df::match(msg, "host"));
	int client = stoi(df::match(msg, "client"));

	// Update host (EventView) score
	df::WorldManager &world_manager = df::WorldManager::getInstance();
	df::EventView ev_host("Host", host, false);
	world_manager.onEvent(&ev_host);

	// Update client (EventView) score
	df::EventView ev_client("Client", client, false);
	world_manager.onEvent(&ev_client);
}



/*
 * rec_create(std::string)
 *	Extracts object id and type from message and creates
 *	that object with a fixed client id offset
 */
void rec_create(std::string msg)
{
	/*	toDo:
	 *		Add isHost boolean value to message from host to
	 *		get rid of this static variable.
	 */
	static bool isHost = true;

	// Extract object ID & type
	int id = std::stoi(df::match(msg, "id")) + ID_OFFSET;
	std::string type = df::match(msg, "type");

	// Create object
	df::Object *newObj = NULL;
	if(type == HERO)
	{
		newObj = new Hero(isHost);
		printf("Created Hero: isHost = (%d) : ID[%d]\n", isHost, id-ID_OFFSET);
		isHost = false;

	} else {
		newObj = new Saucer;
		newObj->deserialize(msg);
	}

	// Set object ID
	newObj->setId(id);
}



/*
 * rec_delete(std::string msg)
 *	Extracts object ID to mark for deletion
 */
void rec_delete(std::string msg)
{
	// Extract object ID
	int id = std::stoi(df::match(msg, "id")) + ID_OFFSET;

	// Use worl manager to get object using id
	df::WorldManager &world_manager = df::WorldManager::getInstance();
	df::Object *del_object = world_manager.objectWithId(id);

	// If object's id is not found
	if(del_object == NULL)
	{
		// Log event and return
		printf("objectWithId(%d) returned NULL\n", id);
		return;
	}

	// Mark object for deletion
	world_manager.markForDelete(del_object);
}



/*
 * rec_update(std::string msg)
 *	Extract Hero id and position and update
 *	Local instance of that player
 *
 *	ASSUMPTION: Only Heros are ever updated, however, this function
 *				allows any object position to be updated
 */
void rec_update(std::string msg)
{
	int id = std::stoi(df::match(msg, "id")) + ID_OFFSET;
	df::WorldManager &world_manager = df::WorldManager::getInstance();
	df::Object *up_object = world_manager.objectWithId(id);

	int x = stoi(df::match(msg, "pos-x"));
	int y = stoi(df::match(msg, "pos-y"));

	df::Position new_pos;
	new_pos.setX(x);
	new_pos.setY(y);
	
	world_manager.moveObject(up_object, new_pos);

	return;
}



/*
 * send_mouse(int x, int y, int id)
 *	Send mouse click coordinates and player ID to peer.
 *
 *	ASSUMPTION: Since this is a multiplayer game, we only need to
 *				send mouse clicks (bullets), no need to send mouse moves
 */
void send_mouse(int x, int y, int id)
{
	// Convert input arguements to string
	std::string ss;
	std::string mouse_x = std::to_string(x);
	std::string mouse_y = std::to_string(y);
	std::string hero_id = std::to_string(id);

	// Concatenate strings into message
	ss += "event:" + MOUSE + ",";
	ss += "mouse_x:" + mouse_x + ",";
	ss += "mouse_y:" + mouse_y + ",";
	ss += "id:" + hero_id + ",}";

	// Send message to peer
	package_message(ss);
	fputs("Sending Mouse Event\n", stdout);
}



/*
 * send_key(char key, int id)
 *	send keyboard event and object id to peer
 */
void send_key(char key, int id)
{
	// Convert if to string
	std::string ss;
	std::string id_str = std::to_string(id);

	// Concatenate input args to message string
	ss += "event:" + KEY + ",";
	ss += "id:" + id_str + ",";
	ss += "key:" + std::string(1, key) + ",";

	// Send message to peer
	package_message(ss);
}



/*
 * package_message(std::string)
 *	This function, used by both client and server
 *	adds a message size header to a message string
 *	and sends that message to it's peer.
 */
void package_message(std::string ss)
{
	const char *temp = ss.c_str();
	int msg_size = ss.size();

	char msg[msg_size+5];
	sprintf(msg, "%3d%s", msg_size, temp);

	df::NetworkManager &network_manager = df::NetworkManager::getInstance();
	network_manager.send(msg, msg_size+3);
}