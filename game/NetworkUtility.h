//
// NetworkUtilitity.h
//
// Network helper functions
//
//
#pragma once


#include <string>
#include "Object.h"

// #define CREATE 0
// #define UPDATE 1
// #define DELETE 2
// #define MOUSE  3
// #define KEY    4

#define ID_OFFSET 1000

const std::string CREATE = "0";
const std::string UPDATE = "1";
const std::string DELETE = "2";
const std::string MOUSE  = "3";
const std::string KEY    = "4";
const std::string SCORE  = "5";

const std::string HERO = "Hero";
const std::string SAUCER = "Saucer";
const std::string BULLET = "Bullet";



/* Host functions:
 *	Handle network functionality regarding
 *	creation, deletion, and updation of objects
 *	as well as receiving key presses and mouse events
 */

// Send serialized object to client
void send_event(df::Object *obj, std::string event);
// Receives message and displatches function to handle it
int receiveHost();
// Handles keyboard events
void rec_key(std::string msg);
// Send score update to client
void send_score(int host, int client);







/* Client Functions:
 *	Handle network functionality regarding
 *	the reception of an object to create, update
 *	or delete as well as sending key presses and mouse events
 */

// Receives message and dispatches function to handle it
int receiveClient();

// Handles Object creation (id, position)
void rec_create(std::string msg);
// Handles Object updates (id, position: only heros need updating)
void rec_update(std::string msg);
// Handles Object deletion (id only)
void rec_delete(std::string msg);
// Sends keyboard events to host
void send_key(char key, int id);
// receive score update
void rec_score(std::string msg);







/*
 * The following functions are used by both client and host
 * 	rec_mouse(): handles the reception of a mouse event
 				 It uses Role in order to attach bullets
 				 to the correct user

 	send_mouse(): Is generic and used to send mouse events

 	package_message(): This function takes in a string of anything
 				 (This game uses "key:value," pairs) and adds a size
 				 header to it before sending it to its peer
 */
// Handles mouse events
void rec_mouse(std::string msg);
// Sends mouse events to host
void send_mouse(int x, int y, int id);
// Adds message header and sends bytes
void package_message(std::string ss);
