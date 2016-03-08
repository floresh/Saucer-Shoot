#include "Object.h"
#include "EventCollision.h"

class Bullet : public df::Object {
private:
	void out();
	void hit(const df::EventCollision *p_collision_event);

public:
	bool hostBullet;
	bool isHostBullet();
	Bullet(df::Position hero_pos, bool isHost);
	int eventHandler(const df::Event *p_e);
};