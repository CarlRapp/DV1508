#ifndef WINDOWCREATOR_H
#define WINDOWCREATOR_H

#include <SDL/SDL.h>
#include "ECSL/Framework/World.h"

namespace WindowCreator
{
	void DECLSPEC CreateMainWindow(ECSL::World* _world);
	void DECLSPEC UpdateWindow(float _dt);
}

#endif