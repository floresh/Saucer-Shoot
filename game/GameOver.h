#include "ViewObject.h"
#include "pthread.h"

class GameOver : public df::ViewObject {
private:
	int time_to_live;
	void step();
	void draw();

public:
	GameOver();
	~GameOver();
	int eventHandler(const df::Event *p_e);
};

