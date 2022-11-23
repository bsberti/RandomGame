#pragma once
#include "GraphicScene.h"
#include "cLightManager.h"

#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"
#include "FModManager.h"
#include <string>

class cRandomUI {
public:
	cRandomUI();

	float masterVolume;
	float musicVolume;
	float fxVolume;
	FModManager* fmod_manager_;

	int iniciatingUI();

	bool DisplayChannelVolume(std::string channelName);
	bool DisplayChannelPan(std::string channelName);
	bool DisplayChannelPitch(std::string channelName);
	bool DisplayChannelPitchDSP(std::string channelName);
	bool DisplayChannelEcho(std::string channelName);
	bool DisplayChannelFader(std::string channelName);

	//void render(GraphicScene &scene);
	void render(GraphicScene&, std::vector<cLight>& vecTheLights);

	int listbox_lights_current;
	int listbox_item_current;
};