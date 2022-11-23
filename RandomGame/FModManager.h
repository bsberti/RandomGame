#pragma once

#include <cstdio>
#include <FMOD/fmod.hpp>
#include <FMOD/fmod_errors.h>
#include <pugixml/pugixml.hpp>
#include <map>
#include <glm/vec3.hpp>
#include <string>
#include <vector>

class FModManager
{
public:
	struct ChannelGroup {
		FMOD::ChannelGroup* group_ptr;
		float current_pan;
		float dsp_pitch;
		float echo;
		float fader;

		bool bEcho, bFader;
		bool echo_enabled;
		bool fader_enabled;

		ChannelGroup() : group_ptr(nullptr), 
			current_pan(0.0f), 
			dsp_pitch(1.0f), 
			echo(500.0f), 
			fader(0.0f),
			bEcho(false),
			bFader(false),
			echo_enabled(false),
			fader_enabled(false) {}
	};

	struct MySound {
		std::string category;
		std::string name;
		std::string path;
		//FMOD_DEFAULT = 0
		//FMOD_LOOP_NORMAL = 2
		int mode;
	};

	FMOD::Channel* bgChannel;

protected:
	FMOD_RESULT last_result_;
	FMOD::System* system_;

	pugi::xml_document soundListDoc;
	std::vector<MySound> mSoundList;

	std::map<std::string, ChannelGroup*> mChannelGroups;	
	std::map<std::string, FMOD::Sound*> mSounds;
	std::map<std::string, FMOD::DSP*> mDSPs;

	bool is_okay(const bool show_error = true) const;

public:
	int choosenAudio;

	//lifecycle
	FModManager();
	bool Initialize(const int number_of_channels, const int system_flags);
	void Shutdown();
	int LoadSounds();

	//channel groups
	bool create_channel_group(const std::string& name);
	bool find_channel_group(const std::string& name, ChannelGroup** channel_group);
	void remove_channel_group(const std::string& name);
	bool set_channel_group_parent(const std::string& child_name, const std::string& parent_name);
	
	bool get_channel_group_volume(const std::string& name, float* volume);
	bool set_channel_group_volume(const std::string& name, float volume);

	bool get_channel_group_pan(const std::string& name, float* pan);
	bool set_channel_group_pan(const std::string& name, float pan);

	bool get_channel_group_enabled(const std::string& name, bool* enabled);
	bool set_channel_group_enabled(const std::string& name, bool enabled);

	//sounds
	bool create_sound(const std::string& name, const std::string& path, FMOD_MODE mode);
	bool play_sound(const std::string& sound_name, glm::vec3 position, float max_distance, float atten, FMOD::Channel** channel);
	bool play_sound(const std::string& name);
	unsigned int getSoundPosition(const std::string& sound_name, const std::string& channel_group_name);

	//dsp
	bool create_dsp(const std::string& name, FMOD_DSP_TYPE dsp_type, const float value);
	bool get_dsp(const std::string& name, FMOD::DSP** dsp);
	bool add_dsp_effect(const std::string& channel_group_name, const std::string& effect_name);
	bool remove_dsp_effect(const std::string& channel_group_name, const std::string& effect_name);

	//3d sounds
	bool set_listener_position(const glm::vec3 position);
	bool update_3d_sound_position(FMOD::Channel* channel, const glm::vec3 position, glm::vec3 vel);
	bool update_sound_volume(FMOD::Channel* channel, const float new_volume);

	bool tick(const glm::vec3& camera_position);
};