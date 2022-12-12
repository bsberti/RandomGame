#include "FModManager.h"
#include <iostream>


bool FModManager::is_okay(const bool show_error) const
{
	if (last_result_ != FMOD_OK) {
		printf("fmod error: #%d-%s\n", last_result_, FMOD_ErrorString(last_result_));
	}

	return last_result_ == FMOD_OK;
}

FModManager::FModManager() : last_result_(FMOD_OK), system_(nullptr), radioOn(false) {}

bool FModManager::Initialize(const int number_of_channels, const int system_flags)
{
	last_result_ = System_Create(&system_);
	if (!is_okay()) {
		return false;
	}

	last_result_ = system_->init(number_of_channels, system_flags, nullptr);
	if (!is_okay()) {
		return false;
	}

	return true;
}

void FModManager::stopRadio(int radioChoice) {
	radioOn = false;
	radioPercentage = 0;
	radioPaused = false;
	radioPlaying = false;
	radioStarving = false;

	currentRadio = mRadios[radioChoice];
	FMOD::Sound* _sound = getSound(currentRadio);
	FMOD::ChannelGroup* _channelGroup = getChannelGroup("radio");
	do
	{
		_result = system_->update();
		assert(!_result);

		_result = _sound->getOpenState(&_openstate, nullptr, nullptr, nullptr);
		assert(!_result);

		Sleep(50);

	} while (_openstate != FMOD_OPENSTATE_READY);
}

void FModManager::Shutdown() {

	for (auto iterator = mDSPs.begin(); iterator != mDSPs.end(); ++iterator) {
		iterator->second->release();
	}
	mDSPs.clear();

	for (auto iterator = mSounds.begin(); iterator != mSounds.end(); ++iterator) {
		iterator->second->release();
	}
	mSounds.clear();

	for (auto iterator = mChannelGroups.begin(); iterator != mChannelGroups.end(); ++iterator) {
		iterator->second->group_ptr->release();
	}
	mChannelGroups.clear();

	if (system_) {
		system_->release();
		system_ = nullptr;
	}
}

int FModManager::LoadSounds() {
	//Load all sounds
	pugi::xml_parse_result result = soundListDoc.load_file("sounds/soundList.xml");
	if (!result) {
		printf("Failed to load 'soundList.xml'\n");
		return -5;
	}

	pugi::xml_object_range<pugi::xml_node_iterator> sounds = soundListDoc.child("soundlist").children();
	if (sounds.empty()) {
		printf("There is no sounds in the soundList\n");
		return -5;
	}

	pugi::xml_node_iterator soundIt;
	for (soundIt = sounds.begin(); soundIt != sounds.end(); soundIt++) {
		pugi::xml_node soundNode = *soundIt;
		
		MySound* currentSound = new MySound();
		pugi::xml_attribute category = soundNode.attribute("category");
		currentSound->category = category.value();

		pugi::xml_node_iterator soundInfoIt;
		for (soundInfoIt = soundNode.children().begin(); 
			soundInfoIt != soundNode.children().end(); 
			soundInfoIt++) {
			pugi::xml_node soundInfoNode = *soundInfoIt;
			
			std::string currentNodeName = soundInfoNode.name();
			if (currentNodeName == "name")
				currentSound->name = soundInfoNode.child_value();

			if (currentNodeName == "mode")
				currentSound->mode = std::stoi(soundInfoNode.child_value());

			if (choosenAudio == 1) {
				if (currentNodeName == "mp3")
					currentSound->path = soundInfoNode.child_value();
			}
			else {
				if (currentNodeName == "wav")
					currentSound->path = soundInfoNode.child_value();
			}
		}

		if (currentSound->category == "radio") {
			mRadios.push_back(currentSound->name);
		}

		//create sounds object
		if (!create_sound(currentSound->name, currentSound->path, currentSound->mode))
			return -5;

	}

	return 0;
}

// Create a new channel Group and use the name as a key for the MAP
bool FModManager::create_channel_group(const std::string& name)
{
	FMOD::ChannelGroup* channel_group;
	last_result_ = system_->createChannelGroup(name.c_str(), &channel_group);
	if (!is_okay()) {
		return false;
	}

	ChannelGroup* cg = new ChannelGroup();
	cg->group_ptr = channel_group;

	mChannelGroups.try_emplace(name, cg);

	return true;
}

bool FModManager::find_channel_group(const std::string& name, ChannelGroup** channel_group) {
	const auto iterator = mChannelGroups.find(name);
	if (iterator == mChannelGroups.end()) {
		return false;
	}

	*channel_group = iterator->second;

	return true;
}

void FModManager::remove_channel_group(const std::string& name) {
	const auto iterator = mChannelGroups.find(name);
	if (iterator == mChannelGroups.end())
	{
		return;
	}

	iterator->second->group_ptr->release();
	mChannelGroups.erase(iterator);
}

bool FModManager::set_channel_group_parent(const std::string& child_name, const std::string& parent_name) {
	const auto child_group = mChannelGroups.find(child_name);
	const auto parent_group = mChannelGroups.find(parent_name);

	if (child_group == mChannelGroups.end() || parent_group == mChannelGroups.end())
	{
		return false;
	}

	last_result_ = parent_group->second->group_ptr->addGroup(child_group->second->group_ptr);

	return true;
}

bool FModManager::get_channel_group_volume(const std::string& name, float* volume) {
	const auto iterator = mChannelGroups.find(name);
	if (iterator == mChannelGroups.end())
	{
		return false;
	}

	return is_okay(iterator->second->group_ptr->getVolume(volume));
}

bool FModManager::set_channel_group_volume(const std::string& name, float volume) {
	const auto iterator = mChannelGroups.find(name);
	if (iterator == mChannelGroups.end())
	{
		return false;
	}

	return is_okay(iterator->second->group_ptr->setVolume(volume));
}

bool FModManager::get_channel_group_pan(const std::string& name, float* pan) {
	const auto iterator = mChannelGroups.find(name);
	if (iterator == mChannelGroups.end())
	{
		return false;
	}

	*pan = iterator->second->current_pan;

	return true;
}

bool FModManager::set_channel_group_pan(const std::string& name, const float pan) {
	const auto iterator = mChannelGroups.find(name);
	if (iterator == mChannelGroups.end())
	{
		return false;
	}

	if (!is_okay(iterator->second->group_ptr->setPan(pan)))
	{
		return false;
	}

	return true;
}

bool FModManager::get_channel_group_enabled(const std::string& name, bool* enabled) {
	const auto iterator = mChannelGroups.find(name);
	if (iterator == mChannelGroups.end())
	{
		return false;
	}

	if (!is_okay(iterator->second->group_ptr->getMute(enabled)))
	{
		return false;
	}

	*enabled = !(*enabled);

	return true;
}

bool FModManager::set_channel_group_enabled(const std::string& name, bool enabled) {
	const auto iterator = mChannelGroups.find(name);
	if (iterator == mChannelGroups.end())
	{
		return false;
	}

	if (!is_okay(iterator->second->group_ptr->setMute(!enabled)))
	{
		return false;
	}

	return true;
}

bool FModManager::create_sound(const std::string& name, const std::string& path, FMOD_MODE mode) {
	FMOD::Sound* sound;
	last_result_ = system_->createSound(path.c_str(), mode, nullptr, &sound);
	if (!is_okay())
	{
		return false;
	}

	mSounds.try_emplace(name, sound);

	return true;
}

unsigned int FModManager::getSoundPosition(const std::string& sound_name, const std::string& channel_group_name) {
	
	unsigned int i = 0;
	bgChannel->getPosition(&i, FMOD_TIMEUNIT_MS);

	return i;
}

FMOD::Sound* FModManager::getSound(const std::string& sound_name)
{
	const auto sound = mSounds.find(sound_name);
	if (sound == mSounds.end()) {
		return nullptr;
	}
	return sound->second;
}

FMOD::ChannelGroup* FModManager::getChannelGroup(const std::string& channel_name)
{
	const auto channelGroup = mChannelGroups.find(channel_name);
	if (channelGroup == mChannelGroups.end()) {
		return nullptr;
	}
	//FMOD::Channel** channel;
	//channelGroup->second->group_ptr->getChannel(0, channel);
	return channelGroup->second->group_ptr;
}

bool FModManager::play_sound(const std::string& sound_name, glm::vec3 position, float max_distance, float atten, FMOD::Channel** channel)
{
	const auto sound = mSounds.find(sound_name);
	if (sound == mSounds.end()) {
		return false;
	}

	if (!is_okay(system_->playSound(sound->second, nullptr, true, channel))) {
		return false;
	}

	FMOD_VECTOR fmod_sound_position;
	fmod_sound_position.x = position.x;
	fmod_sound_position.y = position.y;
	fmod_sound_position.z = position.z;

	if (!is_okay((*channel)->set3DAttributes(&fmod_sound_position, nullptr))) {
		return false;
	}

	//min distance to hear @ max volume
	//max distance where the attenuation stops
	if (!is_okay((*channel)->set3DMinMaxDistance(max_distance, atten))) {
		return false;
	}

	if (!is_okay((*channel)->setPaused(false))) {
		return false;
	}

	return true;
}

bool FModManager::play_sound(const std::string& sound_name, const std::string& channel_group_name) {
	const auto sound_iterator = mSounds.find(sound_name);
	const auto channel_group_iterator = mChannelGroups.find(channel_group_name);

	if (sound_iterator == mSounds.end() || channel_group_iterator == mChannelGroups.end()) {
		return false;
	}

	//channel_group_iterator->second->group_ptr->setPaused(false);
	last_result_ = system_->playSound(sound_iterator->second, channel_group_iterator->second->group_ptr, true, &bgChannel);
	if (!is_okay()) {
		return false;
	}

	last_result_ = (*bgChannel).setPaused(false);
	if (!is_okay()) {
		return false;
	}

	return true;
}

bool FModManager::pause_sound(const std::string& sound_name, const std::string& channel_group_name)
{
	const auto sound_iterator = mSounds.find(sound_name);
	const auto channel_group_iterator = mChannelGroups.find(channel_group_name);

	if (sound_iterator == mSounds.end() || channel_group_iterator == mChannelGroups.end()) {
		return false;
	}

	//channel_group_iterator->second->group_ptr->setPaused(true);
	//last_result_ = system_->playSound(sound_iterator->second, channel_group_iterator->second->group_ptr, false, &bgChannel);
	//if (!is_okay()) {
	//	return false;
	//}
	

	last_result_ = (*bgChannel).setPaused(true);
	if (!is_okay()) {
		return false;
	}

	return true;
}

bool FModManager::play_sound(const std::string& name)
{
	const auto sound = mSounds.find(name);
	if (sound == mSounds.end())
	{
		return false;
	}

	FMOD::Channel* channel;
	if (!is_okay(system_->playSound(sound->second, nullptr, false, &channel)))
	{
		return false;
	}

	return true;
}

// For DSP Creation we need a name for the map key, a DSP TYPE and the value
// values can be blank and set as default
bool FModManager::create_dsp(const std::string& name, FMOD_DSP_TYPE dsp_type, const float value) {
	FMOD::DSP* dsp;

	//figure out the kind we are creating

	if (!is_okay(system_->createDSPByType(dsp_type, &dsp))) {
		return false;
	}

	if(!is_okay(dsp->setParameterFloat(0, value)))
	{
		return false;
	}

	mDSPs.try_emplace(name, dsp);
	return true;
}

bool FModManager::get_dsp(const std::string& name, FMOD::DSP** dsp)
{
	const auto dsp_effect_iterator = mDSPs.find(name);
	if (dsp_effect_iterator == mDSPs.end())
	{
		return false;
	}

	*dsp = dsp_effect_iterator->second;

	return true;
}

bool FModManager::add_dsp_effect(const std::string& channel_group_name, const std::string& effect_name) {
	const auto channel_group_iterator = mChannelGroups.find(channel_group_name);
	const auto dsp_effect_iterator = mDSPs.find(effect_name);
	if (channel_group_iterator == mChannelGroups.end() || dsp_effect_iterator == mDSPs.end())
	{
		return false;
	}

	int num_dsp;
	if (!is_okay(channel_group_iterator->second->group_ptr->getNumDSPs(&num_dsp)))
	{
		return false;
	}

	if (!is_okay(channel_group_iterator->second->group_ptr->addDSP(num_dsp, dsp_effect_iterator->second)))
	{
		return false;
	}

	return true;
}

bool FModManager::remove_dsp_effect(const std::string& channel_group_name, const std::string& effect_name) {
	const auto channel_group_iterator = mChannelGroups.find(channel_group_name);
	const auto dsp_effect_iterator = mDSPs.find(effect_name);
	if (channel_group_iterator == mChannelGroups.end() || dsp_effect_iterator == mDSPs.end())
	{
		return false;
	}

	if (!is_okay(channel_group_iterator->second->group_ptr->removeDSP(dsp_effect_iterator->second)))
	{
		return false;
	}

	return true;
}

bool FModManager::set_listener_position(const glm::vec3 position)
{
	FMOD_VECTOR fmod_position;
	fmod_position.x = position.x;
	fmod_position.y = position.y;
	fmod_position.z = position.z;

	return is_okay(system_->set3DListenerAttributes(0, &fmod_position, nullptr, nullptr, nullptr));
}

bool FModManager::update_3d_sound_position(FMOD::Channel* channel, const glm::vec3 position, glm::vec3 vel)
{
	FMOD_VECTOR fmod_position;
	fmod_position.x = position.x;
	fmod_position.y = position.y;
	fmod_position.z = position.z;

	FMOD_VECTOR fmodVel;
	fmodVel.x = vel.x;
	fmodVel.y = vel.y;
	fmodVel.z = vel.z;
	return is_okay(channel->set3DAttributes(&fmod_position, &fmodVel));
}

bool FModManager::update_sound_volume(FMOD::Channel* channel, const float new_volume)
{
	return is_okay(channel->setVolume(new_volume));
}

bool FModManager::tick(const glm::vec3& camera_position)
{
	//update listener to camera position
	FMOD_VECTOR fmod_camera_position;
	fmod_camera_position.x = camera_position.x;
	fmod_camera_position.y = camera_position.y;
	fmod_camera_position.z = camera_position.z;

	is_okay(system_->set3DListenerAttributes(0, &fmod_camera_position, nullptr, nullptr, nullptr));

	return is_okay(system_->update());
}

//std::string FModManager::getCurrentRadio()
//{
//	std::string current = "";
//	for (std::map<std::string, bool>::iterator itCurrentRadio = mRadios.begin();
//		itCurrentRadio != mRadios.end();
//		itCurrentRadio++) {
//		if (itCurrentRadio->second == true)
//			current = itCurrentRadio->first.c_str();
//	}
//
//	if (current == "") {
//		mRadios["jazz"] = true;
//		//mRadios.try_emplace("jazz", true);
//		current = "jazz";
//	}
//	return current;
//}
