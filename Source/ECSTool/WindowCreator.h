#ifndef WINDOWCREATOR_H
#define WINDOWCREATOR_H

#include <SDL/SDL.h>
#include "ECSL/Framework/World.h"
#include "Renderer/GraphicDevice.h"

namespace WindowCreator
{
	void DECLSPEC CreateMainWindow(ECSL::World* _world, Renderer::GraphicDevice* _graphics);
	void DECLSPEC UpdateWindow(float _dt);
}

#endif