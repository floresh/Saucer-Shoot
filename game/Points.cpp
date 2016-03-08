#include "Points.h"
#include "Event.h"
#include "EventStep.h"
#include "ViewObject.h"

Points::Points()
{
	setLocation(df::TOP_RIGHT);
	setViewString(POINTS_STRING);
	setColor(df::YELLOW);

	// Need to update score each second
	registerInterest(df::STEP_EVENT);
}


int Points::eventHandler(const df::Event *p_e)
{
	// Parent handles event if score update
	if(df::ViewObject::eventHandler(p_e)) {
		return 1;
	}
	if(p_e->getType() == df::STEP_EVENT)
	{
		if(dynamic_cast <const df::EventStep *> (p_e) -> getStepCount() % 30 == 0) {
			setValue(getValue() + 1);
		}
		return 1;
	}
}