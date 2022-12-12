
#include "globalOpenGL.h"

#include <glm/glm.hpp>
#include <glm/vec3.hpp> 
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp> 
#include <glm/gtc/matrix_transform.hpp> 
#include <glm/gtc/type_ptr.hpp> 
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <sstream>

#include <vector>
#include "globalThings.h"
#include "cShaderManager.h"
#include "cVAOManager/cVAOManager.h"
#include "cLightHelper.h"
#include "cVAOManager/c3DModelFileLoader.h"
#include "GraphicScene.h"

#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"

#include "ParticleSystem.h"
#include "Particle.h"
#include "cRandomUI.h"
#include "SimulationView.h"
#include "FModManager.h"
#include "BlocksLoader.h"
#include "cBasicTextureManager/cBasicTextureManager.h"

BlocksLoader* m_blocksLoader;

glm::vec3 g_cameraEye = glm::vec3(0.0f, 0.0f, 0.0f);
glm::vec3 g_cameraTarget = glm::vec3(0.0f, 0.0f, 0.0f);

cBasicTextureManager* g_pTextureManager = NULL;

cVAOManager* pVAOManager;
GraphicScene g_GraphicScene;
ParticleSystem g_ParticleSystem;
cRandomUI gameUi;
SimulationView* simView = new SimulationView();
GLFWwindow* window;
//initialize our sound manager
FModManager* fmod_manager;

FMOD::Channel* _channel;
constexpr int max_channels = 255;

#define NUMBER_OF_TAGS 10
#define MAP_WIDTH 50
#define MAP_HEIGHT 50
#define GLOBAL_MAP_OFFSET 15

// Call back signatures here
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);

void DrawObject(cMeshObject* pCurrentMeshObject,
    glm::mat4x4 mat_PARENT_Model,               // The "parent's" model matrix
    GLuint shaderID,                            // ID for the current shader
    cBasicTextureManager* pTextureManager,
    cVAOManager* pVAOManager,
    GLint mModel_location,                      // Uniform location of mModel matrix
    GLint mModelInverseTransform_location);      // Uniform location of mView location


static void error_callback(int error, const char* description) {
    fprintf(stderr, "Error: %s\n", description);
}

// From here: https://stackoverflow.com/questions/5289613/generate-random-float-between-two-floats/5289624
float RandomFloat(float a, float b) {
    float random = ((float)rand()) / (float)RAND_MAX;
    float diff = b - a;
    float r = random * diff;
    return a + r;
}


// ------------------------ Load Model into VAO ------------------------
bool LoadModelTypesIntoVAO(std::string fileTypesToLoadName,
    cVAOManager* pVAOManager,
    GLuint shaderID)
{
    std::ifstream modelTypeFile(fileTypesToLoadName.c_str());
    if (!modelTypeFile.is_open()) {
        // Can't find that file
        return false;
    }

    // At this point, the file is open and ready for reading
    std::string PLYFileNameToLoad;      // example = "assets/models/MOTO/Blender (load from OBJ export) - only Moto_xyz_n_rgba_uv.ply";
    std::string friendlyName;           // example = "MOTO";

    bool bKeepReadingFile = true;

    const unsigned int BUFFER_SIZE = 1000;  // 1000 characters
    char textBuffer[BUFFER_SIZE];           // Allocate AND clear (that's the {0} part)
    // Clear that array to all zeros
    memset(textBuffer, 0, BUFFER_SIZE);

    while (bKeepReadingFile) {
        // Reads the entire line into the buffer (including any white space)
        modelTypeFile.getline(textBuffer, BUFFER_SIZE);

        PLYFileNameToLoad.clear();  // Erase whatever is already there (from before)
        PLYFileNameToLoad.append(textBuffer);

        // Is this the end of the file (have I read "EOF" yet?)?
        if (PLYFileNameToLoad == "EOF") {
            bKeepReadingFile = false;
            continue;
        }

        // Load the "friendly name" line also
        memset(textBuffer, 0, BUFFER_SIZE);
        modelTypeFile.getline(textBuffer, BUFFER_SIZE);
        friendlyName.clear();
        friendlyName.append(textBuffer);

        sModelDrawInfo drawInfo;
        c3DModelFileLoader fileLoader;

        std::string errorText = "";
        if (fileLoader.LoadPLYFile_Format_XYZ_N_RGBA_UV(PLYFileNameToLoad, drawInfo, errorText)) {
            std::cout << "Loaded the file OK" << std::endl;
        }
        else {
            std::cout << errorText;
        }

        if (pVAOManager->LoadModelIntoVAO(friendlyName, drawInfo, shaderID)) {
            std::cout << "Loaded the " << friendlyName << " model" << std::endl;
        }
    }

    return true;
}
// ------------------------ Load Model into VAO ------------------------

bool SaveTheVAOModelTypesToFile(std::string fileTypesToLoadName,
    cVAOManager* pVAOManager);

void DrawConcentricDebugLightObjects(int currentLight);

// HACK: These are the light spheres we will use for debug lighting
cMeshObject* pDebugSphere_1 = NULL;// = new cMeshObject();
cMeshObject* pDebugSphere_2 = NULL;// = new cMeshObject();
cMeshObject* pDebugSphere_3 = NULL;// = new cMeshObject();
cMeshObject* pDebugSphere_4 = NULL;// = new cMeshObject();
cMeshObject* pDebugSphere_5 = NULL;// = new cMeshObject();

void lightning(GLuint shaderID) {
    ::g_pTheLightManager = new cLightManager();
    cLightHelper* pLightHelper = new cLightHelper();

    ::g_pTheLightManager->CreateBasicDirecLight(shaderID, glm::vec4(250.0f, 900.0f, 250.0f, 0.0f));
    //::g_pTheLightManager->CreateBasicSpotLight(shaderID, glm::vec4(103.0f, 7.0f, 67.0f, 0.0f));
}

void creatingModels() {
    sModelDrawInfo drawingInformation;
    for (int i = 0; i < m_blocksLoader->g_blockMap->size(); i++) {
        for (int j = 0; j < m_blocksLoader->g_blockMap->at(i).size(); j++) {
            std::string currentString = m_blocksLoader->g_blockMap->at(i).at(j);
            if (currentString != "") {
                if (currentString == "X") {
                    pVAOManager->FindDrawInfoByModelName("Cube", drawingInformation);
                    float x = (j * GLOBAL_MAP_OFFSET);
                    float z = (i * GLOBAL_MAP_OFFSET);
                    g_GraphicScene.CreateGameObjectByType("Cube", glm::vec3(x, 0.0f, z), drawingInformation);
                }
              }
        }
    }   
}

void debugLightSpheres() {
    pDebugSphere_1 = new cMeshObject();
    pDebugSphere_1->meshName = "ISO_Sphere_1";
    pDebugSphere_1->friendlyName = "Debug Sphere 1";
    pDebugSphere_1->bUse_RGBA_colour = true;
    pDebugSphere_1->RGBA_colour = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
    pDebugSphere_1->isWireframe = true;
    pDebugSphere_1->scale = 1.0f;
    pDebugSphere_1->bDoNotLight = true;
    g_GraphicScene.vec_pMeshObjects.push_back(pDebugSphere_1);

    pDebugSphere_2 = new cMeshObject();
    pDebugSphere_2->meshName = "ISO_Sphere_1";
    pDebugSphere_2->friendlyName = "Debug Sphere 2";
    pDebugSphere_2->bUse_RGBA_colour = true;
    pDebugSphere_2->RGBA_colour = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
    pDebugSphere_2->isWireframe = true;
    pDebugSphere_2->scale = 1.0f;
    pDebugSphere_2->bDoNotLight = true;
    g_GraphicScene.vec_pMeshObjects.push_back(pDebugSphere_2);

    pDebugSphere_3 = new cMeshObject();
    pDebugSphere_3->meshName = "ISO_Sphere_1";
    pDebugSphere_3->friendlyName = "Debug Sphere 3";
    pDebugSphere_3->bUse_RGBA_colour = true;
    pDebugSphere_3->RGBA_colour = glm::vec4(0.0f, 1.0f, 0.0f, 1.0f);
    pDebugSphere_3->isWireframe = true;
    pDebugSphere_3->scale = 1.0f;
    pDebugSphere_3->bDoNotLight = true;
    g_GraphicScene.vec_pMeshObjects.push_back(pDebugSphere_3);

    pDebugSphere_4 = new cMeshObject();
    pDebugSphere_4->meshName = "ISO_Sphere_1";
    pDebugSphere_4->friendlyName = "Debug Sphere 4";
    pDebugSphere_4->bUse_RGBA_colour = true;
    pDebugSphere_4->RGBA_colour = glm::vec4(0.0f, 1.0f, 1.0f, 1.0f);
    pDebugSphere_4->isWireframe = true;
    pDebugSphere_4->scale = 1.0f;
    pDebugSphere_4->bDoNotLight = true;
    g_GraphicScene.vec_pMeshObjects.push_back(pDebugSphere_4);

    pDebugSphere_5 = new cMeshObject();
    pDebugSphere_5->meshName = "ISO_Sphere_1";
    pDebugSphere_5->friendlyName = "Debug Sphere 5";
    pDebugSphere_5->bUse_RGBA_colour = true;
    pDebugSphere_5->RGBA_colour = glm::vec4(0.0f, 1.0f, 1.0f, 1.0f);
    pDebugSphere_5->isWireframe = true;
    pDebugSphere_5->scale = 1.0f;
    pDebugSphere_5->bDoNotLight = true;
    g_GraphicScene.vec_pMeshObjects.push_back(pDebugSphere_5);
}

void calculateTrianglesCenter(cMeshObject* obj) {
    g_GraphicScene.trianglesCenter.reserve(obj->numberOfTriangles);
    sModelDrawInfo drawingInformation;
    pVAOManager->FindDrawInfoByModelName(obj->meshName, drawingInformation);

    for (int i = 0; i < obj->numberOfTriangles; i++) {        
        glm::vec3 triangleCenter1;
        glm::vec3 triangleCenter2;
        /*if ((drawingInformation.pVertices[(int)obj->meshTriangles[i].x].y ==
            drawingInformation.pVertices[(int)obj->meshTriangles[i].y].y) &&
            (drawingInformation.pVertices[(int)obj->meshTriangles[i].x].y ==
            drawingInformation.pVertices[(int)obj->meshTriangles[i].z].y)) {*/
            
            triangleCenter1.x = (
                drawingInformation.pVertices[(int)obj->meshTriangles[i].x].x +
                drawingInformation.pVertices[(int)obj->meshTriangles[i].y].x +
                drawingInformation.pVertices[(int)obj->meshTriangles[i].z].x) / 3;

            //triangleCenter1.y = drawingInformation.pVertices[(int)obj->meshTriangles[i].x].y;

            triangleCenter1.y = (
                drawingInformation.pVertices[(int)obj->meshTriangles[i].x].y +
                drawingInformation.pVertices[(int)obj->meshTriangles[i].y].y +
                drawingInformation.pVertices[(int)obj->meshTriangles[i].z].y) / 3;

            triangleCenter1.z = (
                drawingInformation.pVertices[(int)obj->meshTriangles[i].x].z +
                drawingInformation.pVertices[(int)obj->meshTriangles[i].y].z +
                drawingInformation.pVertices[(int)obj->meshTriangles[i].z].z) / 3;
        /*}*/

        g_GraphicScene.trianglesCenter.push_back(triangleCenter1);
        obj->trianglesCenter.push_back(triangleCenter1);
    }
}

void positioningObjects() {


    for (int i = 0; i < g_GraphicScene.vec_pMeshObjects.size(); i++) {
        cMeshObject* currObj = g_GraphicScene.vec_pMeshObjects[i];
        if (currObj->friendlyName != "Terrain_Florest" && 
            currObj->friendlyName != "Plane") {
            g_GraphicScene.PositioningMe(currObj);
        }
    }
}

int main(int argc, char* argv[]) {
    std::cout << "starting up..." << std::endl;

    GLuint vertex_buffer = 0;
    GLuint shaderID = 0;
    GLint vpos_location = 0;
    GLint vcol_location = 0;

    int updateCount = 0;

    m_blocksLoader = new BlocksLoader(MAP_HEIGHT, MAP_WIDTH);

    // ------------------ FMOD INITIALIZATION ------------------------------------
    {
    //initialize fmod with max channels
        fmod_manager = new FModManager();
        if (!fmod_manager->Initialize(max_channels, FMOD_INIT_NORMAL)) {
            std::cout << "Failed to initialize FMod" << std::endl;
            return -1;
        }

        // 1 - MP3 Format
        // 2 - WAV Format
        fmod_manager->choosenAudio = 1;
        // Load XML soundList and create sounds
        if (fmod_manager->LoadSounds() != 0) {
            std::cout << "Failed to load sounds in sounds/sounds.xml" << std::endl;
            return -5;
        }

        //create channel groups
        if (
            !fmod_manager->create_channel_group("master") ||
            !fmod_manager->create_channel_group("music") ||
            !fmod_manager->create_channel_group("fx") ||
            !fmod_manager->create_channel_group("radio")
            )
        {
            return -2;
        }

        //set parents for channel groups
        if (!fmod_manager->set_channel_group_parent("music", "master") ||
            !fmod_manager->set_channel_group_parent("fx", "master") ||
            !fmod_manager->set_channel_group_parent("radio", "master")
            )
            return -4;
    }
    // ------------------ FMOD INICIALIZATION ------------------------------------

    glfwSetErrorCallback(error_callback);

    if (!glfwInit())
        exit(EXIT_FAILURE);

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

    window = glfwCreateWindow(1024, 768, "Random Game", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    std::cout << "Window created." << std::endl;

    glfwSetKeyCallback(window, key_callback);

    glfwMakeContextCurrent(window);
    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
    glfwSwapInterval(1);

    gameUi.fmod_manager_ = fmod_manager;
    gameUi.iniciatingUI();

    // Create a shader thingy
    cShaderManager* pTheShaderManager = new cShaderManager();

    cShaderManager::cShader vertexShader01;
    cShaderManager::cShader fragmentShader01;

    vertexShader01.fileName = "assets/shaders/vertexShader01.glsl";
    fragmentShader01.fileName = "assets/shaders/fragmentShader01.glsl";

    if (!pTheShaderManager->createProgramFromFile("Shader_1", vertexShader01, fragmentShader01)) {
        std::cout << "Didn't compile shaders" << std::endl;
        std::string theLastError = pTheShaderManager->getLastError();
        std::cout << "Because: " << theLastError << std::endl;
        return -1;
    }
    else {
        std::cout << "Compiled shader OK." << std::endl;
    }

    pTheShaderManager->useShaderProgram("Shader_1");
    shaderID = pTheShaderManager->getIDFromFriendlyName("Shader_1");
    glUseProgram(shaderID);

    // Setting the lights
    lightning(shaderID);

    pVAOManager = new cVAOManager();
    if (!LoadModelTypesIntoVAO("assets/PLYFilesToLoadIntoVAO.txt", pVAOManager, shaderID)) {
        std::cout << "Error: Unable to load list of models to load into VAO file" << std::endl;
        // Do we exit here? 
        // TO-DO
    }

    // Load the models
    creatingModels();

    debugLightSpheres();

    GLint mvp_location = glGetUniformLocation(shaderID, "MVP"); 
    GLint mModel_location = glGetUniformLocation(shaderID, "mModel");
    GLint mView_location = glGetUniformLocation(shaderID, "mView");
    GLint mProjection_location = glGetUniformLocation(shaderID, "mProjection");
    // Need this for lighting
    GLint mModelInverseTransform_location = glGetUniformLocation(shaderID, "mModelInverseTranspose");

    g_cameraTarget = glm::vec3(250.f, 0.0, 250.f);
    g_cameraEye = glm::vec3(250.f, 1000.f, 260.f);

    // ---------------- GAME LOOP START -----------------------------------------------
    while (!glfwWindowShouldClose(window)) {
        ::g_pTheLightManager->CopyLightInformationToShader(shaderID);

        DrawConcentricDebugLightObjects(gameUi.listbox_lights_current);

        float ratio;
        int width, height;

        glm::mat4x4 matProjection;
        glm::mat4x4 matView;

        glfwGetFramebufferSize(window, &width, &height);
        ratio = width / (float)height;

        glViewport(0, 0, width, height);

        // note the binary OR (not the usual boolean "or" comparison)
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        //glClear(GL_COLOR_BUFFER_BIT);

        glm::vec3 upVector = glm::vec3(0.0f, 1.0f, 0.0f);

        matView = glm::lookAt(::g_cameraEye,
            ::g_cameraTarget,
            upVector);

        // Pass eye location to the shader
        // uniform vec4 eyeLocation;
        GLint eyeLocation_UniLoc = glGetUniformLocation(shaderID, "eyeLocation");

        glUniform4f(eyeLocation_UniLoc,
            ::g_cameraEye.x, ::g_cameraEye.y, ::g_cameraEye.z, 1.0f);

        matProjection = glm::perspective(
            0.6f,           // Field of view (in degress, more or less 180)
            ratio,
            0.1f,           // Near plane (make this as LARGE as possible)
            10000.0f);      // Far plane (make this as SMALL as possible)
                            // 6-8 digits of precision

        // Set once per scene (not per object)
        glUniformMatrix4fv(mView_location, 1, GL_FALSE, glm::value_ptr(matView));
        glUniformMatrix4fv(mProjection_location, 1, GL_FALSE, glm::value_ptr(matProjection));


        //    ____  _             _            __                           
        //   / ___|| |_ __ _ _ __| |_    ___  / _|  ___  ___ ___ _ __   ___ 
        //   \___ \| __/ _` | '__| __|  / _ \| |_  / __|/ __/ _ \ '_ \ / _ \
        //    ___) | || (_| | |  | |_  | (_) |  _| \__ \ (_|  __/ | | |  __/
        //   |____/ \__\__,_|_|   \__|  \___/|_|   |___/\___\___|_| |_|\___|
        //                                                                  
        // We draw everything in our "scene"
        // In other words, go throug the vec_pMeshObjects container
        //  and draw each one of the objects 
        for (std::vector< cMeshObject* >::iterator itCurrentMesh = g_GraphicScene.vec_pMeshObjects.begin();
            itCurrentMesh != g_GraphicScene.vec_pMeshObjects.end();
            itCurrentMesh++)
        {
            cMeshObject* pCurrentMeshObject = *itCurrentMesh;        // * is the iterator access thing

            if (!pCurrentMeshObject->bIsVisible)
            {
                // Skip the rest of the loop
                continue;
            }

            // The parent's model matrix is set to the identity
            glm::mat4x4 matModel = glm::mat4x4(1.0f);

            // All the drawing code has been moved to the DrawObject function
            DrawObject(pCurrentMeshObject,
                matModel,
                shaderID, ::g_pTextureManager,
                pVAOManager, mModel_location, mModelInverseTransform_location);
        }
        //    _____           _          __                           
        //   | ____|_ __   __| |   ___  / _|  ___  ___ ___ _ __   ___ 
        //   |  _| | '_ \ / _` |  / _ \| |_  / __|/ __/ _ \ '_ \ / _ \
        //   | |___| | | | (_| | | (_) |  _| \__ \ (_|  __/ | | |  __/
        //   |_____|_| |_|\__,_|  \___/|_|   |___/\___\___|_| |_|\___|

        glfwPollEvents();

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        
        gameUi.render(g_GraphicScene, fmod_manager, g_pTheLightManager->vecTheLights);

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);

        // Set the window title
        std::stringstream ssTitle;
        ssTitle << "Camera (x,y,z): "
            << ::g_cameraEye.x << ", "
            << ::g_cameraEye.y << ", "
            << ::g_cameraEye.z
            << "  Light #0 (xyz): "
            << ::g_pTheLightManager->vecTheLights[0].position.x << ", "
            << ::g_pTheLightManager->vecTheLights[0].position.y << ", "
            << ::g_pTheLightManager->vecTheLights[0].position.z
            << " linear: " << ::g_pTheLightManager->vecTheLights[0].atten.y
            << " quad: " << ::g_pTheLightManager->vecTheLights[0].atten.z;

        std::string theText = ssTitle.str();

        // Physics Update
        //g_ParticleSystem.Integrate(1.f);
        simView->m_PhysicsSystem.UpdateStep(0.1f);

        glfwSetWindowTitle(window, ssTitle.str().c_str());
    }

    // Get rid of stuff
    delete pTheShaderManager;
    delete ::g_pTheLightManager;

    glfwDestroyWindow(window);
    fmod_manager->Shutdown();
    delete fmod_manager;

    glfwTerminate();
    exit(EXIT_SUCCESS);
}

void DrawConcentricDebugLightObjects(int currentLight) {
    extern bool bEnableDebugLightingObjects;

    if (!bEnableDebugLightingObjects)
    {
        pDebugSphere_1->bIsVisible = false;
        pDebugSphere_2->bIsVisible = false;
        pDebugSphere_3->bIsVisible = false;
        pDebugSphere_4->bIsVisible = false;
        pDebugSphere_5->bIsVisible = false;
        return;
    }

    pDebugSphere_1->bIsVisible = true;
    pDebugSphere_2->bIsVisible = true;
    pDebugSphere_3->bIsVisible = true;
    pDebugSphere_4->bIsVisible = true;
    pDebugSphere_5->bIsVisible = true;

    cLightHelper theLightHelper;

    // Move the debug sphere to where the light #0 is
    pDebugSphere_1->position = glm::vec3(::g_pTheLightManager->vecTheLights[currentLight].position);
    pDebugSphere_2->position = glm::vec3(::g_pTheLightManager->vecTheLights[currentLight].position);
    pDebugSphere_3->position = glm::vec3(::g_pTheLightManager->vecTheLights[currentLight].position);
    pDebugSphere_4->position = glm::vec3(::g_pTheLightManager->vecTheLights[currentLight].position);
    pDebugSphere_5->position = glm::vec3(::g_pTheLightManager->vecTheLights[currentLight].position);

    {
        // Draw a bunch of concentric spheres at various "brightnesses" 
        float distance75percent = theLightHelper.calcApproxDistFromAtten(
            0.75f,  // 75%
            0.001f,
            100000.0f,
            ::g_pTheLightManager->vecTheLights[currentLight].atten.x,
            ::g_pTheLightManager->vecTheLights[currentLight].atten.y,
            ::g_pTheLightManager->vecTheLights[currentLight].atten.z);

        pDebugSphere_2->scale = distance75percent;
        pDebugSphere_2->position = glm::vec3(::g_pTheLightManager->vecTheLights[currentLight].position);
    }

    {
        // Draw a bunch of concentric spheres at various "brightnesses" 
        float distance50percent = theLightHelper.calcApproxDistFromAtten(
            0.50f,  // 75%
            0.001f,
            100000.0f,
            ::g_pTheLightManager->vecTheLights[currentLight].atten.x,
            ::g_pTheLightManager->vecTheLights[currentLight].atten.y,
            ::g_pTheLightManager->vecTheLights[currentLight].atten.z);

        pDebugSphere_3->scale = distance50percent;
        pDebugSphere_3->position = glm::vec3(::g_pTheLightManager->vecTheLights[currentLight].position);
    }

    {
        // Draw a bunch of concentric spheres at various "brightnesses" 
        float distance25percent = theLightHelper.calcApproxDistFromAtten(
            0.25f,  // 75%
            0.001f,
            100000.0f,
            ::g_pTheLightManager->vecTheLights[currentLight].atten.x,
            ::g_pTheLightManager->vecTheLights[currentLight].atten.y,
            ::g_pTheLightManager->vecTheLights[currentLight].atten.z);

        pDebugSphere_4->scale = distance25percent;
        pDebugSphere_4->position = glm::vec3(::g_pTheLightManager->vecTheLights[currentLight].position);
    }

    {
        // Draw a bunch of concentric spheres at various "brightnesses" 
        float distance5percent = theLightHelper.calcApproxDistFromAtten(
            0.05f,  // 75%
            0.001f,
            100000.0f,
            ::g_pTheLightManager->vecTheLights[currentLight].atten.x,
            ::g_pTheLightManager->vecTheLights[currentLight].atten.y,
            ::g_pTheLightManager->vecTheLights[currentLight].atten.z);

        pDebugSphere_5->scale = distance5percent;
        pDebugSphere_5->position = glm::vec3(::g_pTheLightManager->vecTheLights[currentLight].position);
    }
    return;
}