#include "ViewObject.h"
#include "pthread.h"

class GameStart : public df::ViewObject {
private:
	bool hasStarted;
	void start();

public:
	GameStart();
	int eventHandler(const df::Event *p_e);
	void draw();
	void ClientHandler();
};


