#pragma once
#include "GraphicScene.h"
#include "cLightManager.h"

#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"

#include <string>

class cRandomUI {
public:
	cRandomUI();
	
	int iniciatingUI();

	//void render(GraphicScene &scene);
	void render(GraphicScene&, std::vector<cLight> &vecTheLights);

	int listbox_lights_current;
	int listbox_item_current;
};