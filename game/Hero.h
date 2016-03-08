#include "Object.h"
#include "Reticle.h"
#include "EventMouse.h"
#include "EventKeyboard.h"

class Hero : public df::Object {

private:
	bool hostHero;
	int nuke_count;
	Reticle *p_reticle;
	int fire_slowdown, fire_countdown;
	int move_slowdown, move_countdown;
	void move(int dy);
	void step();
	void mouse(const df::EventMouse *p_mouse_event);
	void nuke();

public:
	Hero(bool isHost);
	~Hero();
	void fire(df::Position target, bool hostBullet);
	int eventHandler(const df::Event *p_e);
	void kbd(const df::EventKeyboard *p_keyboard_event);
};