#
# Makefile for saucer shoot game using Dragonfly
#
# Copyright Mark Claypool and WPI, 2015
#
# 'make depend' to generate new dependency list
# 'make clean' to remove all constructed files
# 'make' to build executable
#
# Variables of interest:
#   GAMESRC is the source code files for the game
#   GAME is the game main() source
#

CC= g++ 

### Uncomment only 1 of the below! ###

# 1) Uncomment below for Linux (64-bit)
LINKLIB= -ldragonfly-linux64 -lsfml-graphics -lsfml-window -lsfml-system -lsfml-audio -lrt -pthread
LINKDIR= -L../dragonfly/lib/ # path to dragonfly library
INCDIR= -I../dragonfly/include/ # path to dragonfly includes

# 2) Uncomment below for Mac (64-bit)
#LINKLIB= -ldragonfly-mac64 -lsfml-graphics -lsfml-window -lsfml-system -lsfml-audio 
#LINKDIR= -L/usr/local/Cellar/sfml/2.3_1/lib/ -L../dragonfly/lib/ 
#INCLUDE= -I/usr/local/Cellar/sfml/2.3_1/include/ -I../dragonfly/include/

######

GAMESRC= \
		Hero.cpp \
		Saucer.cpp \
		Bullet.cpp \
		Reticle.cpp \
		Explosion.cpp \
		EventNuke.cpp \
		Points.cpp \
		Star.cpp \
		GameOver.cpp \
		GameStart.cpp \
		NetworkManager.cpp \
		EventNetwork.cpp \
		Sentry.cpp \
		Role.cpp \
		NetworkUtility.cpp \

GAME= game.cpp
EXECUTABLE= game
OBJECTS= $(GAMESRC:.cpp=.o)
FLAGS= -std=c++11

all: $(EXECUTABLE) Makefile

$(EXECUTABLE): $(ENGINE) $(OBJECTS) $(GAME) $(GAMESRC) 
	$(CC) $(GAME) $(OBJECTS) -o $@ $(INCDIR) $(LINKDIR) $(LINKLIB) 

.cpp.o: 
	$(CC) -c $(INCDIR) $(FLAGS) $< -o $@

clean:
	rm -rf $(OBJECTS) $(EXECUTABLE) core dragonfly.log Makefile.bak *~

depend: 
	makedepend *.cpp 2> /dev/null

# DO NOT DELETE
