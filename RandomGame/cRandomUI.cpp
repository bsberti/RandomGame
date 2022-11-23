#include "cRandomUI.h"

#include "imgui.h"
#include "imgui_impl_opengl3.h"
#include <stdio.h>
#include <string>

extern GLFWwindow* window;

cRandomUI::cRandomUI() {
    listbox_lights_current = 0;
    listbox_item_current = -1;
    masterVolume = 0;
    musicVolume = 0;
    fxVolume = 0;
    fmod_manager_ = nullptr;
}

int cRandomUI::iniciatingUI()
{
    //initialize imgui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();

    //platform / render bindings
    if (!ImGui_ImplGlfw_InitForOpenGL(window, true) || !ImGui_ImplOpenGL3_Init("#version 460"))
        return 3;

    //imgui style (dark mode for the win)
    ImGui::StyleColorsDark();
    
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

    return 0;
}

bool cRandomUI::DisplayChannelVolume(std::string channelName)
{
    FModManager::ChannelGroup* channel_group;
    if (!fmod_manager_->find_channel_group(channelName, &channel_group)) {
        return false;
    }

    float currentVolume;
    if (!fmod_manager_->get_channel_group_volume(channelName, &currentVolume)) {
        return false;
    }

    currentVolume *= 100;
    ImGui::SliderFloat((channelName + " Volume").c_str(), &currentVolume, 0.0f, 100.0f, "%.0f");
    currentVolume /= 100;

    if (!fmod_manager_->set_channel_group_volume(channelName, currentVolume)) {
        return false;
    }

    bool volume_enabled;
    if (!fmod_manager_->get_channel_group_enabled(channelName, &volume_enabled)) {
        return false;
    }

    ImGui::SameLine();
    ImGui::Checkbox(("##" + channelName + "_volume").c_str(), &volume_enabled);

    if (!fmod_manager_->set_channel_group_enabled(channelName, volume_enabled)) {
        return false;
    }

    return true;
}

bool cRandomUI::DisplayChannelPan(std::string channelName) {
    FModManager::ChannelGroup* channel_group;
    if (!fmod_manager_->find_channel_group(channelName, &channel_group)) {
        return false;
    }

    ImGui::SliderFloat("Pan", &channel_group->current_pan, -1.0f, 1.0f, " % .2f");

    if (!fmod_manager_->set_channel_group_pan(channelName, channel_group->current_pan)) {
        return false;
    }

    return true;
}

bool cRandomUI::DisplayChannelPitch(std::string channelName)
{
    FModManager::ChannelGroup* channel_group;
    if (!fmod_manager_->find_channel_group(channelName, &channel_group)) {
        return false;
    }

    float current_pitch;
    channel_group->group_ptr->getPitch(&current_pitch);
    ImGui::SliderFloat((channelName + " Pitch").c_str(), &current_pitch, 0.5f, 2.0f, "%.2f");
    channel_group->group_ptr->setPitch(current_pitch);

    return true;
}

bool cRandomUI::DisplayChannelPitchDSP(std::string channelName)
{
    FModManager::ChannelGroup* channel_group;
    if (!fmod_manager_->find_channel_group(channelName, &channel_group)) {
        return false;
    }

    ImGui::SliderFloat("Pitch (using dsp)", &channel_group->dsp_pitch, 0.5f, 2.0f, "%.2f");
    FMOD::DSP* dsp;
    if (!fmod_manager_->get_dsp("dsp_pitch", &dsp)) {
        return false;
    }
    dsp->setParameterFloat(0, channel_group->dsp_pitch);
    return true;
}

bool cRandomUI::DisplayChannelEcho(std::string channelName) {
    FModManager::ChannelGroup* channel_group;
    if (!fmod_manager_->find_channel_group(channelName, &channel_group)) {
        return false;
    }

    ImGui::SliderFloat((channelName + " echo").c_str(), &channel_group->echo, 10.f, 5000.0f, "%.2f");
    FMOD::DSP* dsp;
    if (!fmod_manager_->get_dsp("echo", &dsp)) {
        return false;
    }
    dsp->setParameterFloat(0, channel_group->echo);

    ImGui::SameLine();
    ImGui::Checkbox(("##" + channelName + "_echo").c_str(), &channel_group->bEcho);

    if (channel_group->bEcho && !channel_group->echo_enabled) {
        channel_group->echo_enabled = true;
        if (!fmod_manager_->add_dsp_effect(channelName, "echo")) {
            return false;
        }
    }
    else if (!channel_group->bEcho && channel_group->echo_enabled) {
        channel_group->echo_enabled = false;
        if (!fmod_manager_->remove_dsp_effect(channelName, "echo")) {
            return false;
        }
    }

    return true;
}

bool cRandomUI::DisplayChannelFader(std::string channelName) {
    FModManager::ChannelGroup* channel_group;
    if (!fmod_manager_->find_channel_group(channelName, &channel_group)) {
        return false;
    }

    ImGui::SliderFloat((channelName + " fader").c_str(), &channel_group->fader, 0.5f, 2.0f, "%.2f");
    FMOD::DSP* dsp;
    if (!fmod_manager_->get_dsp("fader", &dsp)) {
        return false;
    }
    dsp->setParameterFloat(0, channel_group->fader);

    ImGui::SameLine();
    ImGui::Checkbox(("##" + channelName + "_fader").c_str(), &channel_group->bFader);

    if (channel_group->bFader && !channel_group->fader_enabled) {
        channel_group->fader_enabled = true;
        if (!fmod_manager_->add_dsp_effect(channelName, "fader")) {
            return false;
        }
    }
    else if (!channel_group->bFader && channel_group->fader_enabled) {
        channel_group->fader_enabled = false;
        if (!fmod_manager_->remove_dsp_effect(channelName, "fader")) {
            return false;
        }
    }

    return true;
}

void cRandomUI::render(GraphicScene &scene, std::vector<cLight> &vecTheLights) {
    static float f = 0.0f;
    static int counter = 0;
    bool lightsEnabled;

    ImGui::Begin("Game Objects");
    ImGui::Text("Mesh Objets");
    const int totalObjects = scene.vec_pMeshObjects.size() - 1;
    const char* listbox_items[20];
    for (int i = 0; i < 20; i++) {
        if (i <= totalObjects)
            listbox_items[i] = scene.vec_pMeshObjects[i]->meshName.c_str();
        else
            listbox_items[i] = "Empty";
    }
    
    ImGui::ListBox("1", &listbox_item_current, listbox_items, IM_ARRAYSIZE(listbox_items), 10);

    ImGui::Text("Light Objets");
    const int totalLights = vecTheLights.size() - 1;
    const char* listbox_lights[20];
    for (int i = 0; i < 20; i++) {
        if (i <= totalLights)
            listbox_lights[i] = vecTheLights[i].name.c_str();
        else
            listbox_lights[i] = "Empty";
    }
    
    ImGui::ListBox("2", &listbox_lights_current, listbox_lights, IM_ARRAYSIZE(listbox_lights), 10);

    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
    ImGui::End();

    ImGui::Begin("Selected Object");
    if (listbox_item_current != -1) {
        ImGui::Text(scene.vec_pMeshObjects[listbox_item_current]->meshName.c_str());

        glm::vec3 soPosition = scene.vec_pMeshObjects[listbox_item_current]->position;
        glm::vec3 soRotation = scene.vec_pMeshObjects[listbox_item_current]->rotation;
        float scale = scene.vec_pMeshObjects[listbox_item_current]->scale;


        ImGui::SliderFloat("position.x", &soPosition.x, -500.0f, 500.0f);
        ImGui::SliderFloat("position.y", &soPosition.y, -500.0f, 500.0f);
        ImGui::SliderFloat("position.z", &soPosition.z, -500.0f, 500.0f);

        ImGui::SliderFloat("scale", &scale, -50.0f, 50.0f);

        ImGui::SliderFloat("rotation.x", &soRotation.x, -10.0f, 10.0f);
        ImGui::SliderFloat("rotation.y", &soRotation.y, -10.0f, 10.0f);
        ImGui::SliderFloat("rotation.z", &soRotation.z, -10.0f, 10.0f);

        scene.vec_pMeshObjects[listbox_item_current]->position.x = soPosition.x;
        scene.vec_pMeshObjects[listbox_item_current]->position.y = soPosition.y;
        scene.vec_pMeshObjects[listbox_item_current]->position.z = soPosition.z;
        scene.vec_pMeshObjects[listbox_item_current]->scale = scale;
        scene.vec_pMeshObjects[listbox_item_current]->rotation.x = soRotation.x;
        scene.vec_pMeshObjects[listbox_item_current]->rotation.y = soRotation.y;
        scene.vec_pMeshObjects[listbox_item_current]->rotation.z = soRotation.z;
    }
    else {
        ImGui::Text("No Object Selected");
    }

    ImGui::End();

    ImGui::Begin("Selected Light");
    if (listbox_lights_current != -1) {
        ImGui::Text(vecTheLights[listbox_lights_current].name.c_str());

        glm::vec4 lPosition = vecTheLights[listbox_lights_current].position;
        glm::vec4 lDiffuse = vecTheLights[listbox_lights_current].diffuse;
        glm::vec4 lSpecular = vecTheLights[listbox_lights_current].specular;
        glm::vec4 lAtten = vecTheLights[listbox_lights_current].atten;
        glm::vec4 lDirection = vecTheLights[listbox_lights_current].direction;
        glm::vec4 lParam1 = vecTheLights[listbox_lights_current].param1;
        glm::vec4 lParam2 = vecTheLights[listbox_lights_current].param2;

        //ImGui::SliderFloat("position.x", &lPosition.x, -500.0f, 500.0f);
        ImGui::DragFloat("position.x", &lPosition.x, 1.0f, 0.0f, 0.0f, "%1f");
        //ImGui::SliderFloat("position.y", &lPosition.y, -500.0f, 500.0f);
        ImGui::DragFloat("position.y", &lPosition.y, 1.0f, 0.0f, 0.0f, "%1f");
        //ImGui::SliderFloat("position.z", &lPosition.z, -500.0f, 500.0f);
        ImGui::DragFloat("position.z", &lPosition.z, 1.0f, 0.0f, 0.0f, "%1f");
        //ImGui::SliderFloat("position.w", &lPosition.w, -1.0f, 1.0f);
        ImGui::DragFloat("position.w", &lPosition.w, 1.0f, 0.0f, 0.0f, "%1f");

        //ImGui::SliderFloat("diffuse.x", &lDiffuse.x, -1.0f, 1.0f);
        ImGui::DragFloat("diffuse.x", &lDiffuse.x, 0.01f, 0.0f, 0.0f, "%.2f");
        //ImGui::SliderFloat("diffuse.y", &lDiffuse.y, -1.0f, 1.0f);
        ImGui::DragFloat("diffuse.y", &lDiffuse.y, 0.01f, 0.0f, 0.0f, "%.2f");
        //ImGui::SliderFloat("diffuse.z", &lDiffuse.z, -1.0f, 1.0f);
        ImGui::DragFloat("diffuse.z", &lDiffuse.z, 0.01f, 0.0f, 0.0f, "%.2f");
        //ImGui::SliderFloat("diffuse.w", &lDiffuse.z, -1.0f, 1.0f);
        ImGui::DragFloat("diffuse.w", &lDiffuse.w, 0.01f, 0.0f, 0.0f, "%.2f");

        ImGui::SliderFloat("specular.x", &lSpecular.x, -1.0f, 1.0f);
        ImGui::SliderFloat("specular.y", &lSpecular.y, -1.0f, 1.0f);
        ImGui::SliderFloat("specular.z", &lSpecular.z, -1.0f, 1.0f);
        ImGui::SliderFloat("specular.w", &lSpecular.w, -1.0f, 1.0f);

        //ImGui::SliderFloat("atten.x", &lAtten.x, -1.0f, 1.0f);
        ImGui::DragFloat("atten.x", &lAtten.x, 0.01f, 0.0f, 0.0f, "%.2f");
        //ImGui::SliderFloat("atten.y", &lAtten.y, -1.0f, 1.0f);
        ImGui::DragFloat("atten.y", &lAtten.y, 0.01f, 0.0f, 0.0f, "%.2f");
        //ImGui::SliderFloat("atten.z", &lAtten.z, -1.0f, 1.0f);
        ImGui::DragFloat("atten.z", &lAtten.z, 0.01f, 0.0f, 0.0f, "%.2f");
        //ImGui::SliderFloat("atten.w", &lAtten.w, -1.0f, 1.0f);
        ImGui::DragFloat("atten.w", &lAtten.w, 0.01f, 0.0f, 0.0f, "%.2f");

        //ImGui::SliderFloat("direction.x", &lDirection.x, 0.0f, 0.1f);
        ImGui::DragFloat("direction.x", &lDirection.x, 0.01f, 0.0f, 0.0f, "%.2f");
        //ImGui::SliderFloat("direction.y", &lDirection.y, 0.0f, 0.1f);
        ImGui::DragFloat("direction.y", &lDirection.y, 0.01f, 0.0f, 0.0f, "%.2f");
        //ImGui::SliderFloat("direction.z", &lDirection.z, 0.0f, 0.1f);
        ImGui::DragFloat("direction.z", &lDirection.z, 0.01f, 0.0f, 0.0f, "%.2f");
        //ImGui::SliderFloat("direction.w", &lDirection.w, 0.0f, 0.1f);
        ImGui::DragFloat("direction.w", &lDirection.w, 0.01f, 0.0f, 0.0f, "%.2f");

        ImGui::SliderFloat("param1.x", &lParam1.x, 0.0f, 2.0f);
        ImGui::SliderFloat("param1.y", &lParam1.y, -1.0f, 1.0f);
        ImGui::SliderFloat("param1.z", &lParam1.z, -1.0f, 1.0f);
        ImGui::SliderFloat("param1.w", &lParam1.w, -1.0f, 1.0f);

        if (vecTheLights[listbox_lights_current].param2.x == 1) {
            lightsEnabled = true;
        }
        else {
            lightsEnabled = false;
        }

        std::string light = "Light" + std::to_string(listbox_lights_current);
        ImGui::Checkbox(("##" + light).c_str(), &lightsEnabled);

        if (lightsEnabled) {
            vecTheLights[listbox_lights_current].param2.x = 1;
        }
        else {
            vecTheLights[listbox_lights_current].param2.x = 0;
        }


        vecTheLights[listbox_lights_current].position.x = lPosition.x;
        vecTheLights[listbox_lights_current].position.y = lPosition.y;
        vecTheLights[listbox_lights_current].position.z = lPosition.z;
        vecTheLights[listbox_lights_current].position.w = lPosition.w;

        vecTheLights[listbox_lights_current].diffuse.x = lDiffuse.x;
        vecTheLights[listbox_lights_current].diffuse.y = lDiffuse.y;
        vecTheLights[listbox_lights_current].diffuse.z = lDiffuse.z;
        vecTheLights[listbox_lights_current].diffuse.w = lDiffuse.w;

        vecTheLights[listbox_lights_current].specular.x = lSpecular.x;
        vecTheLights[listbox_lights_current].specular.y = lSpecular.y;
        vecTheLights[listbox_lights_current].specular.z = lSpecular.z;
        vecTheLights[listbox_lights_current].specular.w = lSpecular.w;

        vecTheLights[listbox_lights_current].atten.x = lAtten.x;
        vecTheLights[listbox_lights_current].atten.y = lAtten.y;
        vecTheLights[listbox_lights_current].atten.z = lAtten.z;
        vecTheLights[listbox_lights_current].atten.w = lAtten.w;

        vecTheLights[listbox_lights_current].direction.x = lDirection.x;
        vecTheLights[listbox_lights_current].direction.y = lDirection.y;
        vecTheLights[listbox_lights_current].direction.z = lDirection.z;
        vecTheLights[listbox_lights_current].direction.w = lDirection.w;

        vecTheLights[listbox_lights_current].param1.x = lParam1.x;
        vecTheLights[listbox_lights_current].param1.y = lParam1.y;
        vecTheLights[listbox_lights_current].param1.z = lParam1.z;
        vecTheLights[listbox_lights_current].param1.w = lParam1.w;

        vecTheLights[listbox_lights_current].param2.x = lParam2.x;
        vecTheLights[listbox_lights_current].param2.y = lParam2.y;
        vecTheLights[listbox_lights_current].param2.z = lParam2.z;
        vecTheLights[listbox_lights_current].param2.w = lParam2.w;
    }
    else {
        ImGui::Text("No Object Selected");
    }

    ImGui::End();

    //setup ui structure
    ImGui::Begin("Game Audio Settings");
    ImGui::BulletText("Master Channel");

    if (!DisplayChannelVolume("master")) {
        // Something went wrong, what now?
    }

    if (!DisplayChannelPan("master")) {
        // Something went wrong, what now?
    }

    if (!DisplayChannelPitch("master")) {
        // Something went wrong, what now?
    }

    if (!DisplayChannelPitchDSP("master")) {
        // Something went wrong, what now?
    }

    if (!DisplayChannelEcho("master")) {
        // Something went wrong, what now?
    }

    if (!DisplayChannelFader("master")) {
        // Something went wrong, what now?
    }

    ImGui::BulletText("Music Channel");
    if (!DisplayChannelVolume("music")) {
        // Something went wrong, what now?
    }

    unsigned int position = fmod_manager_->getSoundPosition("piano-bg", "master");
    std::string minuto = std::to_string(position / 1000 / 60);
    std::string segundo = std::to_string(position / 1000 % 60);

    ImGui::Text(("Time Elapsed: " + minuto + ":" + segundo).c_str());

    if (!DisplayChannelPitch("music")) {
        // Something went wrong, what now?
    }

    if (!DisplayChannelEcho("music")) {
        // Something went wrong, what now?
    }

    if (!DisplayChannelFader("music")) {
        // Something went wrong, what now?
    }

    ImGui::BulletText("Sound Effect Channel");
    if (!DisplayChannelVolume("fx")) {
        // Something went wrong, what now?
    }

    if (!DisplayChannelPitch("fx")) {
        // Something went wrong, what now?
    }

    if (!DisplayChannelEcho("fx")) {
        // Something went wrong, what now?
    }

    if (!DisplayChannelFader("fx")) {
        // Something went wrong, what now?
    }

    ImGui::End();
}