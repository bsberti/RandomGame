#include "cRandomUI.h"

#include "imgui.h"
#include "imgui_impl_opengl3.h"
#include <stdio.h>
#include <string>

extern GLFWwindow* window;

cRandomUI::cRandomUI() {
    listbox_lights_current = 0;
    listbox_item_current = -1;
    listbox_child_current = -1;
    listbox_behold_current = -1;
    masterVolume = 0;
    musicVolume = 0;
    fxVolume = 0;
    fmod_manager_ = nullptr;
    radioChoice = 0;
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

void cRandomUI::render(GraphicScene& scene, FModManager* fmod, std::vector<cLight>& vecTheLights) {
    static float f = 0.0f;
    static int counter = 0;
    bool lightsEnabled;

    ImGui::Begin("Game Objects");
    ImGui::Text("Mesh Objets");
    const int totalObjects = scene.vec_pMeshObjects.size() - 1;
    const char* listbox_items[100];
    for (int i = 0; i < 100; i++) {
        if (i <= totalObjects)
            listbox_items[i] = scene.vec_pMeshObjects[i]->friendlyName.c_str();
        else
            listbox_items[i] = "Empty";
    }
    ImGui::ListBox("1", &listbox_item_current, listbox_items, IM_ARRAYSIZE(listbox_items), 20);

    ImGui::Text("Childrens Objects");
    int totalChildrenObjects = 0;

    for (int i = 0; i <= totalObjects; i++) {
        if (scene.vec_pMeshObjects[i]->vecChildMeshes.size() != 0) {
            totalChildrenObjects += scene.vec_pMeshObjects[i]->vecChildMeshes.size();
        }
    }
    
    //const char* listbox_child[40];
    std::vector<std::string> childList;
    for (int i = 0; i < 40; i++) {
        if (scene.vec_pMeshObjects[i]->vecChildMeshes.size() != 0) {
            for (int j = 0; j < scene.vec_pMeshObjects[i]->vecChildMeshes.size(); j++) {
                if (j < totalChildrenObjects) {
                    std::string faterName = scene.vec_pMeshObjects[i]->friendlyName;
                    std::string childName = scene.vec_pMeshObjects[i]->vecChildMeshes[j]->friendlyName;
                    std::string fullname = faterName + childName;
                    childList.push_back(fullname);
                    vecChildMeshes.push_back(scene.vec_pMeshObjects[i]->vecChildMeshes[j]);
                    //listbox_child[i] = scene.vec_pMeshObjects[i]->vecChildMeshes[j]->friendlyName.c_str();
                }
                else {
                    //listbox_child[i] = "Empty";
                }
            }
        }
    }

    const char* listbox_child[50];
    for (int i = 0; i < 50; i++) {
        listbox_child[i] = "Empty";
    }

    for (int i = 0; i < childList.size(); i++) {
        listbox_child[i] = childList.at(i).c_str();
    }

    ImGui::ListBox("2", &listbox_child_current, listbox_child, IM_ARRAYSIZE(listbox_child), 10);

    ImGui::Text("BEHOLDS");
    const char* listbox_beholds[3];
    for (std::map< std::string, cMeshObject*>::iterator itBeholds =
        scene.map_beholds->begin(); itBeholds != scene.map_beholds->end();
        itBeholds++) {
        cMeshObject* currentBehold = itBeholds->second;
        vecBeholds.push_back(currentBehold);
    }

    listbox_beholds[0] = vecBeholds[0]->friendlyName.c_str();
    listbox_beholds[1] = vecBeholds[1]->friendlyName.c_str();
    listbox_beholds[2] = vecBeholds[2]->friendlyName.c_str();
    ImGui::ListBox("3", &listbox_behold_current, listbox_beholds, IM_ARRAYSIZE(listbox_beholds), 3);

    ImGui::Text("Light Objets");
    const int totalLights = vecTheLights.size() - 1;
    const char* listbox_lights[10];
    for (int i = 0; i < 10; i++) {
        if (i <= totalLights)
            listbox_lights[i] = vecTheLights[i].name.c_str();
        else
            listbox_lights[i] = "Empty";
    }
    ImGui::ListBox("4", &listbox_lights_current, listbox_lights, IM_ARRAYSIZE(listbox_lights), 10);

    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
    ImGui::End();

    ImGui::Begin("Selected Object");
    if (listbox_item_current != -1) {
        ImGui::Text(scene.vec_pMeshObjects[listbox_item_current]->meshName.c_str());

        glm::vec3 soPosition = scene.vec_pMeshObjects[listbox_item_current]->position;
        glm::quat soRotation = scene.vec_pMeshObjects[listbox_item_current]->qRotation;
        float scale = scene.vec_pMeshObjects[listbox_item_current]->scale;


        ImGui::SliderFloat("position.x", &soPosition.x, -500.0f, 500.0f);
        ImGui::SliderFloat("position.y", &soPosition.y, -500.0f, 500.0f);
        ImGui::SliderFloat("position.z", &soPosition.z, -500.0f, 500.0f);

        ImGui::SliderFloat("scale", &scale, -50.0f, 50.0f);

        ImGui::SliderFloat("rotation.x", &soRotation.x, -10.0f, 10.0f);
        ImGui::SliderFloat("rotation.y", &soRotation.y, -10.0f, 10.0f);
        ImGui::SliderFloat("rotation.z", &soRotation.z, -10.0f, 10.0f);
        ImGui::SliderFloat("rotation.w", &soRotation.w, -10.0f, 10.0f);

        scene.vec_pMeshObjects[listbox_item_current]->position.x = soPosition.x;
        scene.vec_pMeshObjects[listbox_item_current]->position.y = soPosition.y;
        scene.vec_pMeshObjects[listbox_item_current]->position.z = soPosition.z;
        scene.vec_pMeshObjects[listbox_item_current]->scale = scale;

        scene.vec_pMeshObjects[listbox_item_current]->setRotationFromEuler(glm::vec3(soRotation.x, soRotation.y, soRotation.z));
        //scene.vec_pMeshObjects[listbox_item_current]->rotation.x = soRotation.x;
        //scene.vec_pMeshObjects[listbox_item_current]->rotation.y = soRotation.y;
        //scene.vec_pMeshObjects[listbox_item_current]->rotation.z = soRotation.z;
    }
    else {
        ImGui::Text("No Object Selected");
    }
    ImGui::End();

    ImGui::Begin("Selected Child Object");
    if (listbox_child_current != -1) {
        ImGui::Text(vecChildMeshes[listbox_child_current]->friendlyName.c_str());

        glm::vec3 soPosition = vecChildMeshes[listbox_child_current]->position;
        glm::quat soRotation = vecChildMeshes[listbox_child_current]->qRotation;
        float scale = vecChildMeshes[listbox_child_current]->scale;

        ImGui::SliderFloat("position.x", &soPosition.x, -500.0f, 500.0f);
        ImGui::SliderFloat("position.y", &soPosition.y, -500.0f, 500.0f);
        ImGui::SliderFloat("position.z", &soPosition.z, -500.0f, 500.0f);

        ImGui::SliderFloat("scale", &scale, -50.0f, 50.0f);

        ImGui::SliderFloat("rotation.x", &soRotation.x, -10.0f, 10.0f);
        ImGui::SliderFloat("rotation.y", &soRotation.y, -10.0f, 10.0f);
        ImGui::SliderFloat("rotation.z", &soRotation.z, -10.0f, 10.0f);
        ImGui::SliderFloat("rotation.w", &soRotation.w, -10.0f, 10.0f);

        vecChildMeshes[listbox_child_current]->position.x = soPosition.x;
        vecChildMeshes[listbox_child_current]->position.y = soPosition.y;
        vecChildMeshes[listbox_child_current]->position.z = soPosition.z;
        vecChildMeshes[listbox_child_current]->scaleXYZ = glm::vec3(scale, scale, scale);

        vecChildMeshes[listbox_item_current]->setRotationFromEuler(glm::vec3(soRotation.x, soRotation.y, soRotation.z));
    }
    else {
        ImGui::Text("No Child Object Selected");
    }
    ImGui::End();

    ImGui::Begin("Selected BEHOLD");
    if (listbox_behold_current != -1) {
        ImGui::Text(vecBeholds[listbox_behold_current]->friendlyName.c_str());

        ImGui::Text("I: "); ImGui::SameLine();
        ImGui::Text(std::to_string(vecBeholds[listbox_behold_current]->currentI).c_str()); ImGui::SameLine();
        ImGui::Text("J: "); ImGui::SameLine();
        ImGui::Text(std::to_string(vecBeholds[listbox_behold_current]->currentJ).c_str());

        ImGui::Text("Moving: "); ImGui::SameLine();
        ImGui::Text(std::to_string(vecBeholds[listbox_behold_current]->moving).c_str());

        glm::vec3 soPosition = vecBeholds[listbox_behold_current]->position;
        glm::quat soRotation = vecBeholds[listbox_behold_current]->qRotation;
        //float scale = vecBeholds[listbox_behold_current]->scale;

        ImGui::SliderFloat("position.x", &soPosition.x, -500.0f, 500.0f);
        ImGui::SliderFloat("position.y", &soPosition.y, -500.0f, 500.0f);
        ImGui::SliderFloat("position.z", &soPosition.z, -500.0f, 500.0f);

        //ImGui::SliderFloat("scale", &scale, -50.0f, 50.0f);

        ImGui::SliderFloat("rotation.x", &soRotation.x, -10.0f, 10.0f);
        ImGui::SliderFloat("rotation.y", &soRotation.y, -10.0f, 10.0f);
        ImGui::SliderFloat("rotation.z", &soRotation.z, -10.0f, 10.0f);
        ImGui::SliderFloat("rotation.w", &soRotation.w, -10.0f, 10.0f);

        vecBeholds[listbox_behold_current]->position.x = soPosition.x;
        vecBeholds[listbox_behold_current]->position.y = soPosition.y;
        vecBeholds[listbox_behold_current]->position.z = soPosition.z;
        //vecBeholds[listbox_behold_current]->scaleXYZ = glm::vec3(scale, scale, scale);

        vecBeholds[listbox_behold_current]->setRotationFromEuler(glm::vec3(soRotation.x, soRotation.y, soRotation.z));
    }
    else {
        ImGui::Text("No Child Object Selected");
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

    ImGui::BulletText("Radio Channel");

    for (int i = 0; i < fmod->mRadios.size(); i++) {
        if (fmod->mRadios[i] == fmod->currentRadio)
            radioChoice = i;
    }

    { // Jazz Radio Update
        if (ImGui::RadioButton("Jazz", radioChoice == 0)) {
            //fmod->stopRadio(radioChoice);
            fmod->radioOn = false;
            fmod->pause_sound(fmod->currentRadio, "radio");

            radioChoice = 0;
            fmod->currentRadio = "jazz";
        }
        ImGui::SameLine();
    }

    { // Pop Radio Update
        if (ImGui::RadioButton("Pop", radioChoice == 1)) {
            //fmod->stopRadio(radioChoice);
            fmod->radioOn = false;
            fmod->pause_sound(fmod->currentRadio, "radio");

            radioChoice = 1;
            fmod->currentRadio = "pop";
        }
        ImGui::SameLine();
    }
    
    { //Rock Radio Update
        if (ImGui::RadioButton("Rock", radioChoice == 2)) {
            //fmod->stopRadio(radioChoice);
            fmod->radioOn = false;
            fmod->pause_sound(fmod->currentRadio, "radio");

            radioChoice = 2;
            fmod->currentRadio = "rockRadio";
        }
    }

    if (!DisplayChannelVolume("radio")) {
        // Something went wrong, what now?
    }

    if (!DisplayChannelPitch("radio")) {
        // Something went wrong, what now?
    }

    if (!DisplayChannelEcho("radio")) {
        // Something went wrong, what now?
    }

    if (!DisplayChannelFader("radio")) {
        // Something went wrong, what now?
    }

    ImGui::End();
}