#include "Role.h"
#include "Reticle.h"

#include "EventMouse.h"
#include "WorldManager.h"
#include "GraphicsManager.h"

Reticle::Reticle()
{
	setType("Reticle");
	setSolidness(df::SPECTRAL);

	setAltitude(df::MAX_ALTITUDE); // Make Reticle foreground

	registerInterest(df::MOUSE_EVENT);
}


// Draw retcle on window
void Reticle::draw()
{
	Role &role = Role::getInstance();
	df::GraphicsManager &graphics_manager = df::GraphicsManager::getInstance();

	if(role.isHost()) {
		graphics_manager.drawCh(getPosition(), RETICLE_CHAR, df::RED);
	} else {
		graphics_manager.drawCh(getPosition(), RETICLE_CHAR, df::YELLOW);
	}
}


int Reticle::eventHandler(const df::Event *p_e)
{
	if(p_e->getType() == df::MOUSE_EVENT)
	{
		const df::EventMouse *p_mouse_event = dynamic_cast <const df::EventMouse *> (p_e);
		if(p_mouse_event->getMouseAction() == df::MOVED)
		{
			// Change location to new mouse position
			setPosition(p_mouse_event->getMousePosition());
			return 1;
		}
	}

	return 0;
}