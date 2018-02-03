# Saucer Shoot

 - Author: Heric Flores-Huerta
 - Project 4 : Network Saucer-shooter

## CONTROLS:
For the purpose of testing/running the game on the same computer the host uses W,D,SPACE while the client uses UP_ARROW,DOWN_ARROW,	and N for up movement, down movement and nuking. 

## Build & Run:

`$ make`

`$ ./game [-h | -c <server name | address> ]`

## Description: 

Running game with the `-h` runs the game as a host. It opens to a black screen waiting for a client connection. When the game is run with a	`-c` flag, it expects the next argument to be the server name or server address. Once the client connects both client and server start-up the game environment.

Upon starting up the game world, the host creates all the saucers and heroes and sends the to the client. The client is always ready to receive object creations, deletions and updates. It receives  the starting world state from the host to begin. The only actions that are synchronized are:

+ Heros movement
+ Bullets Creation
+ Saucer Creation & Deletion
+ Hero Creation & Deletion
+ Nukes


