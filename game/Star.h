#include "Object.h"
#define STAR_CHAR '.'

class Star : df::Object {
private:
	void out();

public:
	Star();
	void draw(void);
	int eventHandler(const df::Event *p_e);
};

