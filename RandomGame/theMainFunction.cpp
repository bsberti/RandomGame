
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
#include <math.h>

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
#include "cLuaBrain.h"

BlocksLoader* m_blocksLoader;

glm::vec3 g_cameraEye = glm::vec3(0.0f, 0.0f, 0.0f);
glm::vec3 g_cameraTarget = glm::vec3(0.0f, 0.0f, 0.0f);

cBasicTextureManager* g_pTextureManager = NULL;

cVAOManager* pVAOManager;
GraphicScene g_GraphicScene;
ParticleSystem g_ParticleSystem;
cLuaBrain* pBrain;
cShaderManager* pTheShaderManager;

cRandomUI gameUi;
SimulationView* simView = new SimulationView();
GLFWwindow* window;
//initialize our sound manager
FModManager* fmod_manager;

FMOD::Channel* _channel;
constexpr int max_channels = 255;

// I guess?
float NinetyDegrees = 1.575f;
std::map< std::string, cMeshObject*>::iterator itBeholdsToFollow;

#define NUMBER_OF_TAGS 10
#define MAP_WIDTH 50
#define MAP_HEIGHT 50
#define GLOBAL_MAP_OFFSET 50
#define SMALLEST_DISTANCE 0.1
#define CAMERA_OFFSET 50.0

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
}

void createWall(unsigned int line, unsigned int column, float x, float z, bool horizontal, std::string orientation) {
    sModelDrawInfo drawingInformation;
    pVAOManager->FindDrawInfoByModelName("Wall", drawingInformation);
    g_GraphicScene.CreateGameObjectByType("Wall", glm::vec3(x, 0.0f, z), drawingInformation);
    cMeshObject* wall;
    wall = g_GraphicScene.GetObjectByName("Wall", false);
    wall->friendlyName = "Wall" + std::to_string(line) + "_" + std::to_string(column) + orientation;
    wall->textures[0] = "Dungeons_2_Texture_01_A.bmp";
    wall->textureRatios[0] = 1.0f;
    if (!horizontal)
        wall->setRotationFromEuler(glm::vec3(0.0f, 1.575f, 0.0f));
}

void createTorch(unsigned int line, unsigned int column, glm::vec3 pos, glm::vec3 rotation, std::string orientation) {
    sModelDrawInfo drawingInformation;
    pVAOManager->FindDrawInfoByModelName("Torch", drawingInformation);
    g_GraphicScene.CreateGameObjectByType("Torch", pos, drawingInformation);
    cMeshObject* torch;
    torch = g_GraphicScene.GetObjectByName("Torch", false);
    torch->friendlyName = "Torch" + std::to_string(line) + "_" + std::to_string(column) + orientation;
    torch->textures[0] = "Dungeons_2_Texture_01_A.bmp";
    torch->textureRatios[0] = 1.0f;
    torch->setRotationFromEuler(rotation);

    cMeshObject* flame = new cMeshObject();
    flame->meshName = "Imposter_Base";
    flame->friendlyName = "Flame" + std::to_string(line) + "_" + std::to_string(column) + orientation;
    flame->textures[0] = "fire-torch-texture.bmp";
    flame->textureRatios[0] = 1.0f;

    //flame->setRotationFromEuler(glm::vec3(0.0f));
    flame->SetUniformScale(3.0f);
    //flame->position = pos;
    //flame->position.y += 10;
    //flame->position.x += 10;

    torch->vecChildMeshes.push_back(flame);
    g_GraphicScene.vec_torchFlames.push_back(flame);
}

void createDeadbody(unsigned int line, unsigned int column, glm::vec3 pos, glm::vec3 rotation, std::string orientation) {
    sModelDrawInfo drawingInformation;
    pVAOManager->FindDrawInfoByModelName("Deadbody", drawingInformation);
    g_GraphicScene.CreateGameObjectByType("Deadbody", pos, drawingInformation);
    cMeshObject* deadbody;
    deadbody = g_GraphicScene.GetObjectByName("Deadbody", false);
    deadbody->friendlyName = "Deadbody" + std::to_string(line) + "_" + std::to_string(column) + orientation;
    deadbody->textures[0] = "Dungeons_2_Texture_01_A.bmp";
    deadbody->textureRatios[0] = 1.0f;
    deadbody->setRotationFromEuler(rotation);
    deadbody->SetUniformScale(0.2f);
}

void creatingModels() {
    sModelDrawInfo drawingInformation;

    for (int i = 0; i < m_blocksLoader->g_blockMap->size(); i++) {
        for (int j = 0; j < m_blocksLoader->g_blockMap->at(i).size(); j++) {
            std::string currentString = m_blocksLoader->g_blockMap->at(i).at(j);
            if (currentString != "") {
                if (currentString != ".") {
                    std::string northString = "";
                    std::string westString = "";
                    std::string eastString = "";
                    std::string southString = "";

                    if (i > 0) northString = m_blocksLoader->g_blockMap->at(i - 1).at(j);
                    if (j > 0) westString = m_blocksLoader->g_blockMap->at(i).at(j - 1);
                    if (j < m_blocksLoader->g_blockMap->at(i).size()) eastString = m_blocksLoader->g_blockMap->at(i).at(j + 1);
                    if (i < m_blocksLoader->g_blockMap->size()) southString = m_blocksLoader->g_blockMap->at(i + 1).at(j);
                    
                    float x = (j * GLOBAL_MAP_OFFSET);
                    float z = (i * GLOBAL_MAP_OFFSET);
                    pVAOManager->FindDrawInfoByModelName("Floor", drawingInformation);
                    g_GraphicScene.CreateGameObjectByType("Floor", glm::vec3(x, 0.0f, z), drawingInformation);
                    cMeshObject* currentObject;
                    currentObject = g_GraphicScene.GetObjectByName("Floor", false);
                    std::string floorName = "Floor" + std::to_string(i) + "_" + std::to_string(j);
                    currentObject->friendlyName = floorName;
                    currentObject->textures[0] = "Dungeons_2_Texture_01_A.bmp";
                    currentObject->textureRatios[0] = 1.0f;

                    // Creating some Torches!
                    if (currentString == "T") {
                        glm::vec3 torchN = glm::vec3(0.0f);
                        glm::vec3 torchW = glm::vec3(0.0f);
                        glm::vec3 torchE = glm::vec3(0.0f);
                        glm::vec3 torchS = glm::vec3(0.0f);

                        // Check if there is another Floor - North
                        if (northString == ".") {
                            torchN.x = (GLOBAL_MAP_OFFSET / 2) + (GLOBAL_MAP_OFFSET) * (j - 1);
                            torchN.y = 25.0f;
                            torchN.z = (GLOBAL_MAP_OFFSET) * (i - 1);
                        }

                        // Check if there is another Floor - West
                        if (westString == ".") {
                            torchW.x = (GLOBAL_MAP_OFFSET) * (j - 1);
                            torchW.y = 25.0f;
                            torchW.z = (GLOBAL_MAP_OFFSET / 2) + (GLOBAL_MAP_OFFSET) * (i - 1);
                        }

                        // Check if there is another Floor - East
                        if (eastString == ".") {
                            torchE.x = (GLOBAL_MAP_OFFSET) * (j);
                            torchE.y = 25.0f;
                            torchE.z = (GLOBAL_MAP_OFFSET / 2) + (GLOBAL_MAP_OFFSET) * (i - 1);
                        }

                        // Check if there is another Floor - South
                        if (southString == ".") {
                            torchS.x = (GLOBAL_MAP_OFFSET / 2) + (GLOBAL_MAP_OFFSET) * (j - 1);
                            torchS.y = 25.0f;
                            torchS.z = (GLOBAL_MAP_OFFSET) * (i);
                        }

                        bool validTorch = false;
                        GLuint shaderID = 0;
                        shaderID = pTheShaderManager->getIDFromFriendlyName("Shader_1");
                        do {
                            unsigned int randTorch = rand() % 4;
                            switch (randTorch) {
                            case 0:
                                // North
                                if (torchN != glm::vec3(0.0f)) {
                                    createTorch(i, j, torchN, glm::vec3(0.0f, (NinetyDegrees * 2), 0.0f), "N");
                                    // Creating the spot light too?
                                    ::g_pTheLightManager->CreateBasicPointLight(shaderID, glm::vec4(torchN.x, torchN.y + 10, torchN.z + 10, 0.0f));
                                    validTorch = true;
                                }
                                break;
                            case 1:
                                // West
                                if (torchW != glm::vec3(0.0f)) {
                                    createTorch(i, j, torchW, glm::vec3(0.0f, (NinetyDegrees * -1), 0.0f), "W");
                                    ::g_pTheLightManager->CreateBasicPointLight(shaderID, glm::vec4(torchW.x + 10, torchW.y + 10, torchW.z, 0.0f));
                                    validTorch = true;
                                }
                                break;
                            case 2:
                                // East
                                if (torchE != glm::vec3(0.0f)) {
                                    createTorch(i, j, torchE, glm::vec3(0.0f, (NinetyDegrees), 0.0f), "E");
                                    ::g_pTheLightManager->CreateBasicPointLight(shaderID, glm::vec4(torchE.x - 10, torchE.y + 10, torchE.z, 0.0f));
                                    validTorch = true;
                                }
                                break;
                            case 3:
                                // South
                                if (torchS != glm::vec3(0.0f)) {
                                    createTorch(i, j, torchS, glm::vec3(0.0f), "S");
                                    ::g_pTheLightManager->CreateBasicPointLight(shaderID, glm::vec4(torchS.x, torchS.y + 10, torchS.z - 10, 0.0f));
                                    validTorch = true;
                                }
                                break;
                            }

                        } while (!validTorch);

                        
                    }

                    // Creating some DeadBodies =0
                    if (currentString == "D") {
                        glm::vec3 deadbodyN = glm::vec3(0.0f);
                        glm::vec3 deadbodyW = glm::vec3(0.0f);
                        glm::vec3 deadbodyE = glm::vec3(0.0f);
                        glm::vec3 deadbodyS = glm::vec3(0.0f);

                        // Check if there is another Floor - North
                        if (northString == ".") {
                            deadbodyN.x = (GLOBAL_MAP_OFFSET / 2) + (GLOBAL_MAP_OFFSET) * (j - 1);
                            deadbodyN.y = 0.0f;
                            deadbodyN.z = (GLOBAL_MAP_OFFSET) * (i - 1) + 10;
                        }

                        // Check if there is another Floor - West
                        if (westString == ".") {
                            deadbodyW.x = (GLOBAL_MAP_OFFSET) * (j - 1) + 10;
                            deadbodyW.y = 0.0f;
                            deadbodyW.z = (GLOBAL_MAP_OFFSET / 2) + (GLOBAL_MAP_OFFSET) * (i - 1);
                        }

                        // Check if there is another Floor - East
                        if (eastString == ".") {
                            deadbodyE.x = (GLOBAL_MAP_OFFSET) * (j) - 10;
                            deadbodyE.y = 0.0f;
                            deadbodyE.z = (GLOBAL_MAP_OFFSET / 2) + (GLOBAL_MAP_OFFSET) * (i - 1);
                        }

                        // Check if there is another Floor - South
                        if (southString == ".") {
                            deadbodyS.x = (GLOBAL_MAP_OFFSET / 2) + (GLOBAL_MAP_OFFSET) * (j - 1);
                            deadbodyS.y = 0.0f;
                            deadbodyS.z = (GLOBAL_MAP_OFFSET) * (i) - 10;
                        }

                        bool validDeadbody = false;
                        GLuint shaderID = 0;
                        shaderID = pTheShaderManager->getIDFromFriendlyName("Shader_1");
                        do {
                            unsigned int randTorch = rand() % 4;
                            switch (randTorch) {
                            case 0:
                                // North
                                if (deadbodyN != glm::vec3(0.0f)) {
                                    createDeadbody(i, j, deadbodyN, glm::vec3(0.0f, (NinetyDegrees), 0.0f), "N");
                                    // Creating the spot light too?
                                    validDeadbody = true;
                                }
                                break;
                            case 1:
                                // West
                                if (deadbodyW != glm::vec3(0.0f)) {
                                    createDeadbody(i, j, deadbodyW, glm::vec3(0.0f, (NinetyDegrees * 2), 0.0f), "W");
                                    validDeadbody = true;
                                }
                                break;
                            case 2:
                                // East
                                if (deadbodyE != glm::vec3(0.0f)) {
                                    createDeadbody(i, j, deadbodyE, glm::vec3(0.0f), "E");
                                    validDeadbody = true;
                                }
                                break;
                            case 3:
                                // South
                                if (deadbodyS != glm::vec3(0.0f)) {
                                    createDeadbody(i, j, deadbodyS, glm::vec3(0.0f, (NinetyDegrees * -1), 0.0f), "S");
                                    validDeadbody = true;
                                }
                                break;
                            }

                        } while (!validDeadbody);
                    }

                    // Creating Crystals Clusters (center of tile)
                    if (currentString == "C") {
                        pVAOManager->FindDrawInfoByModelName("Crystal_Cluster1", drawingInformation);
                        g_GraphicScene.CreateGameObjectByType("Crystal_Cluster1", glm::vec3(x - (GLOBAL_MAP_OFFSET / 2), 10.0f, z - (GLOBAL_MAP_OFFSET / 2)), drawingInformation);
                        cMeshObject* crystalC;
                        crystalC = g_GraphicScene.GetObjectByName("Crystal_Cluster1", false);
                        std::string crystalCName = "Crystal_Cluster1" + std::to_string(i) + "_" + std::to_string(j);
                        crystalC->friendlyName = crystalCName;
                        crystalC->bUse_RGBA_colour = true;
                        crystalC->RGBA_colour = glm::vec4(0.4f, 1.0f, 0.8f, 0.75f);
                        crystalC->specular_colour_and_power = glm::vec4(0.4f, 1.0f, 0.8f, 0.75f);
                        crystalC->SetUniformScale(RandomFloat(0.05f, 0.10f));
                        crystalC->setRotationFromEuler(glm::vec3(0.0f, rand() % 180, 0.0f));
                    }

                    // Creating Crystals Clusters (center of tile)
                    if (currentString == "c") {
                        pVAOManager->FindDrawInfoByModelName("Crystal_Cluster2", drawingInformation);
                        g_GraphicScene.CreateGameObjectByType("Crystal_Cluster2", glm::vec3(x - (GLOBAL_MAP_OFFSET / 2), 10.0f, z - (GLOBAL_MAP_OFFSET / 2)), drawingInformation);
                        cMeshObject* crystalC;
                        crystalC = g_GraphicScene.GetObjectByName("Crystal_Cluster2", false);
                        std::string crystalCName = "Crystal_Cluster2" + std::to_string(i) + "_" + std::to_string(j);
                        crystalC->friendlyName = crystalCName;
                        crystalC->bUse_RGBA_colour = true;
                        crystalC->RGBA_colour = glm::vec4(0.5f, 0.1f, 1.0f, 0.8f);
                        crystalC->specular_colour_and_power = glm::vec4(0.5f, 0.1f, 1.0f, 0.3f);
                        crystalC->SetUniformScale(RandomFloat(0.10f, 0.15f));
                        crystalC->setRotationFromEuler(glm::vec3(0.0f, rand() % 180, 0.0f));
                    }

                    // Creating Crystals Clusters (center of tile)
                    if (currentString == "Y") {
                        pVAOManager->FindDrawInfoByModelName("Crystal_Cluster3", drawingInformation);
                        g_GraphicScene.CreateGameObjectByType("Crystal_Cluster3", glm::vec3(x - (GLOBAL_MAP_OFFSET / 2), 10.0f, z - (GLOBAL_MAP_OFFSET / 2)), drawingInformation);
                        cMeshObject* crystalC;
                        crystalC = g_GraphicScene.GetObjectByName("Crystal_Cluster3", false);
                        std::string crystalCName = "Crystal_Cluster3" + std::to_string(i) + "_" + std::to_string(j);
                        crystalC->friendlyName = crystalCName;
                        crystalC->bUse_RGBA_colour = true;
                        crystalC->RGBA_colour = glm::vec4(0.0f, 0.2f, 1.0f, 0.65f);
                        crystalC->specular_colour_and_power = glm::vec4(0.0f, 0.2f, 1.0f, 0.65f);
                        crystalC->SetUniformScale(RandomFloat(0.10f, 0.15f));
                        crystalC->setRotationFromEuler(glm::vec3(0.0f, rand() % 180, 0.0f));
                    }

                    // Creating the mighty BEHOLDER!
                    if (currentString == "B") {
                        pVAOManager->FindDrawInfoByModelName("Beholder", drawingInformation);
                        g_GraphicScene.CreateGameObjectByType("Beholder", glm::vec3(x - (GLOBAL_MAP_OFFSET / 2), 25.0f, z - (GLOBAL_MAP_OFFSET / 2)), drawingInformation);
                        cMeshObject* beholder;
                        beholder = g_GraphicScene.GetObjectByName("Beholder", false);
                        std::string beholderName = "Beholder" + std::to_string(i) + "_" + std::to_string(j);
                        beholder->friendlyName = beholderName;
                        beholder->textures[0] = "Beholder_Base_color.bmp";
                        beholder->textureRatios[0] = 1.0f;
                        beholder->SetUniformScale(10.0f);
                    }

                    // Creating the mighty BEHOLDER!
                    if (currentString == "b") {
                        pVAOManager->FindDrawInfoByModelName("Beholder", drawingInformation);
                        g_GraphicScene.CreateGameObjectByType("Beholder", glm::vec3(x - (GLOBAL_MAP_OFFSET / 2), 25.0f, z - (GLOBAL_MAP_OFFSET / 2)), drawingInformation);
                        cMeshObject* beholder;
                        beholder = g_GraphicScene.GetObjectByName("Beholder", false);
                        std::string beholderName = "B" + std::to_string(i) + "_" + std::to_string(j);
                        beholder->friendlyName = beholderName;
                        beholder->textures[0] = "Beholder_Base_color.bmp";
                        beholder->textureRatios[0] = 1.0f;
                        beholder->SetUniformScale(10.0f);

                        cMeshObject* cone = new cMeshObject();
                        cone->meshName = "Beholder_Vision";
                        cone->friendlyName = "Beholder_Vision" + std::to_string(i) + "_" + std::to_string(j);
                        cone->bUse_RGBA_colour = true;
                        cone->RGBA_colour = glm::vec4(1.0f, 1.0f, 1.0f, 0.5f);
                        cone->bDoNotLight = true;
                        beholder->vecChildMeshes.push_back(cone);

                        beholder->currentI = i;
                        beholder->currentJ = j;

                        g_GraphicScene.map_beholds->try_emplace(beholderName, beholder);
                    }

                    // Creating some Walls! Depending on the adjacents tiles
                    if (i == 0) {
                        if (j == 0) {
                            float x1 = (GLOBAL_MAP_OFFSET / 2) * (i - 1);
                            float z1 = (GLOBAL_MAP_OFFSET) * (j - 1);
                            createWall(i, j, x1, z1, true, "N");

                            float x2 = (GLOBAL_MAP_OFFSET) * (i - 1);
                            float z2 = (GLOBAL_MAP_OFFSET / 2) * (i - 1);
                            createWall(i, j, x2, z2, false, "W");

                            // Check if there is another Floor - East
                            if (eastString == ".") {
                                float x3 = (GLOBAL_MAP_OFFSET) * (i);
                                float z3 = (GLOBAL_MAP_OFFSET / 2) * (i - 1);
                                createWall(i, j, x3, z3, false, "E");
                            }

                            // Check if there is another Floor - South
                            if (southString == ".") {
                                float x4 = (GLOBAL_MAP_OFFSET / 2) * (i - 1);
                                float z4 = (GLOBAL_MAP_OFFSET / 2) * (i);
                                createWall(i, j, x4, z4, true, "S");
                            }

                        }
                        else {
                            float x1 = (GLOBAL_MAP_OFFSET / 2) + GLOBAL_MAP_OFFSET * (j - 1);
                            float z1 = (GLOBAL_MAP_OFFSET) * (i - 1);
                            createWall(i, j, x1, z1, true, "N");

                            // Check if there is another Floor - West
                            if (westString == ".") {
                                float x2 = (GLOBAL_MAP_OFFSET) * (i);
                                float z2 = (GLOBAL_MAP_OFFSET / 2) * (i - 1);
                                createWall(i, j, x2, z2, false, "W");
                            }

                            // Check if there is another Floor - East
                            if (eastString == ".") {
                                float x3 = (GLOBAL_MAP_OFFSET) * (j);
                                float z3 = (GLOBAL_MAP_OFFSET / 2) * (i - 1);
                                createWall(i, j, x3, z3, false, "E");
                            }

                            // Check if there is another Floor - South
                            if (southString == ".") {
                                float x4 = (GLOBAL_MAP_OFFSET / 2) + GLOBAL_MAP_OFFSET * (j - 1);
                                float z4 = GLOBAL_MAP_OFFSET * i;
                                createWall(i, j, x4, z4, true, "S");
                            }
                        }
                    }
                    else {
                        if (j == 0) {
                            // Check if there is another Floor - North
                            if (northString == ".") {
                                float x1 = (GLOBAL_MAP_OFFSET / 2) * (j - 1);
                                float z1 = (GLOBAL_MAP_OFFSET) * (i - 1);
                                createWall(i, j, x1, z1, true, "N");
                            }

                            float x2 = (GLOBAL_MAP_OFFSET) * (j - 1);
                            float z2 = (GLOBAL_MAP_OFFSET / 2) + GLOBAL_MAP_OFFSET * (i - 1);
                            createWall(i, j, x2, z2, false, "W");

                            // Check if there is another Floor - East
                            if (eastString == ".") {
                                float x3 = (GLOBAL_MAP_OFFSET) * (j);
                                float z3 = (GLOBAL_MAP_OFFSET / 2) + GLOBAL_MAP_OFFSET * (i - 1);
                                createWall(i, j, x3, z3, false, "E");
                            }

                            // Check if there is another Floor - South
                            if (southString == ".") {
                                float x4 = (GLOBAL_MAP_OFFSET / 2) * (j - 1);
                                float z4 = (GLOBAL_MAP_OFFSET) * (i);
                                createWall(i, j, x4, z4, true, "S");
                            }
                        }
                        else {
                            // Check if there is another Floor - North
                            if (northString == ".") {
                                float x1 = (GLOBAL_MAP_OFFSET / 2) + (GLOBAL_MAP_OFFSET) * (j - 1);
                                float z1 = (GLOBAL_MAP_OFFSET) * (i - 1);
                                createWall(i, j, x1, z1, true, "N");
                            }

                            // Check if there is another Floor - West
                            if (westString == ".") {
                                float x2 = (GLOBAL_MAP_OFFSET) * (j - 1);
                                float z2 = (GLOBAL_MAP_OFFSET / 2) + (GLOBAL_MAP_OFFSET) * (i - 1);
                                createWall(i, j, x2, z2, false, "W");
                            }

                            // Check if there is another Floor - East
                            if (eastString == ".") {
                                float x3 = (GLOBAL_MAP_OFFSET) * (j);
                                float z3 = (GLOBAL_MAP_OFFSET / 2) + (GLOBAL_MAP_OFFSET) * (i - 1);
                                createWall(i, j, x3, z3, false, "E");
                            }

                            // Check if there is another Floor - South
                            if (southString == ".") {
                                float x4 = (GLOBAL_MAP_OFFSET / 2) + (GLOBAL_MAP_OFFSET) * (j - 1);
                                float z4 = (GLOBAL_MAP_OFFSET) * (i);
                                createWall(i, j, x4, z4, true, "S");
                            }
                        }
                    }
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
    pDebugSphere_1->RGBA_colour = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
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

bool calculateNextPosition(cMeshObject* currentBehold, glm::vec3& nextPosition) {

    std::string northString = "";
    std::string westString = "";
    std::string eastString = "";
    std::string southString = "";

    unsigned int i = currentBehold->currentI;
    unsigned int j = currentBehold->currentJ;

    if (i > 0) northString = m_blocksLoader->g_blockMap->at(i - 1).at(j);
    if (j > 0) westString = m_blocksLoader->g_blockMap->at(i).at(j - 1);
    if (j < m_blocksLoader->g_blockMap->at(i).size()) eastString = m_blocksLoader->g_blockMap->at(i).at(j + 1);
    if (i < m_blocksLoader->g_blockMap->size()) southString = m_blocksLoader->g_blockMap->at(i + 1).at(j);

    glm::vec3 returnVec;
    returnVec = currentBehold->position;

    for (std::map< std::string, cMeshObject*>::iterator itBeholds =
        g_GraphicScene.map_beholds->begin(); itBeholds != g_GraphicScene.map_beholds->end();
        itBeholds++) {
        cMeshObject* cmBehold = itBeholds->second;
        unsigned int cmBeholdID = cmBehold->getID();

        if (cmBeholdID == currentBehold->getID()) continue;

        // N = i - 1 and j
        // NE = i - 1 and j + 1
        // E = i and j + 1
        // SE = i + 1 and j + 1
        // S = i + 1 and j
        // SW = j + 1 and j - 1
        // W = i and j - 1
        // NW = i - 1 and j - 1
        std::string cmBeholdIJString = std::to_string(cmBehold->currentI) + std::to_string(cmBehold->currentJ);

        std::string N = std::to_string(i - 1) + std::to_string(j);
        std::string NE = std::to_string(i - 1) + std::to_string(j + 1);
        std::string E = std::to_string(i) + std::to_string(j + 1);
        std::string SE = std::to_string(i + 1) + std::to_string(j + 1);
        std::string S = std::to_string(i + 1) + std::to_string(j);
        std::string SW = std::to_string(i + 1) + std::to_string(j - 1);
        std::string W = std::to_string(i) + std::to_string(j - 1);
        std::string NW = std::to_string(i - 1) + std::to_string(j - 1);

        if (cmBeholdIJString == N || cmBeholdIJString == NE || cmBeholdIJString == E || cmBeholdIJString == SE ||
            cmBeholdIJString == S || cmBeholdIJString == SW || cmBeholdIJString == W || cmBeholdIJString == NW) {
            // ATTACK!
            return false;
        }
    }

    bool validPath = false;
    do {
        unsigned int randPath = rand() % 4;
        switch (randPath) {
        case 0:
            // North
            if (northString == "X" || northString == "D" || northString == "T" ||
                northString == "b" || northString == "B") {
                returnVec.z -= 50.0f;
                currentBehold->currentI -= 1;
                validPath = true;
            }
            break;
        case 1:
            // West
            if (westString == "X" || westString == "D" || westString == "T" ||
                westString == "b" || westString == "B") {
                returnVec.x -= 50.0f;
                currentBehold->currentJ -= 1;
                validPath = true;
            }
            break;
        case 2:
            // East
            if (eastString == "X" || eastString == "D" || eastString == "T" ||
                eastString == "b" || eastString == "B") {
                returnVec.x += 50.0f;
                currentBehold->currentJ += 1;
                validPath = true;
            }
            break;
        case 3:
            // South
            if (southString == "X" || southString == "D" || southString == "T" ||
                southString == "b" || southString == "B") {
                returnVec.z += 50.0f;
                currentBehold->currentI += 1;
                validPath = true;
            }
            break;
        }

    } while (!validPath);

    nextPosition = returnVec;
    return true;
}

int main(int argc, char* argv[]) {
    std::cout << "starting up..." << std::endl;

    GLuint vertex_buffer = 0;
    GLuint shaderID = 0;
    GLint vpos_location = 0;
    GLint vcol_location = 0;

    srand(time(NULL));

    int updateCount = 0;
    pBrain = new cLuaBrain();
    g_GraphicScene.map_beholds = new std::map<std::string, cMeshObject*>();

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
    pTheShaderManager = new cShaderManager();

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
    itBeholdsToFollow = g_GraphicScene.map_beholds->begin();

    debugLightSpheres();

    GLint mvp_location = glGetUniformLocation(shaderID, "MVP"); 
    GLint mModel_location = glGetUniformLocation(shaderID, "mModel");
    GLint mView_location = glGetUniformLocation(shaderID, "mView");
    GLint mProjection_location = glGetUniformLocation(shaderID, "mProjection");
    // Need this for lighting
    GLint mModelInverseTransform_location = glGetUniformLocation(shaderID, "mModelInverseTranspose");

    cMeshObject* pSkyBox = new cMeshObject();
    pSkyBox->meshName = "Skybox_Sphere";
    pSkyBox->friendlyName = "skybox";

    // ---------------- LOADING TEXTURES ----------------------------------------------
    ::g_pTextureManager = new cBasicTextureManager();
    ::g_pTextureManager->SetBasePath("assets/textures");

    if (!::g_pTextureManager->Create2DTextureFromBMPFile("Dungeons_2_Texture_01_A.bmp")) {
        std::cout << "Didn't load texture" << std::endl;
    }
    else {
        std::cout << "texture loaded" << std::endl;
    }

    if (!::g_pTextureManager->Create2DTextureFromBMPFile("lroc_color_poles_4k.bmp")) {
        std::cout << "Didn't load texture" << std::endl;
    }
    else {
        std::cout << "texture loaded" << std::endl;
    }

    if (!::g_pTextureManager->Create2DTextureFromBMPFile("fire-torch-texture.bmp")) {
        std::cout << "Didn't load texture" << std::endl;
    }
    else {
        std::cout << "texture loaded" << std::endl;
    }

    if (!::g_pTextureManager->Create2DTextureFromBMPFile("Beholder_Base_color.bmp")) {
        std::cout << "Didn't load texture" << std::endl;
    }
    else {
        std::cout << "texture loaded" << std::endl;
    }

    // Load a skybox
    // Here's an example of the various sides: http://www.3dcpptutorials.sk/obrazky/cube_map.jpg
    std::string errorString = "";
    if (::g_pTextureManager->CreateCubeTextureFromBMPFiles("SpaceBox",
        "SpaceBox_right1_posX.bmp", /* positive X */
        "SpaceBox_left2_negX_1.bmp",  /* negative X */
        "SpaceBox_top3_posY.bmp",    /* positive Y */
        "SpaceBox_bottom4_negY.bmp",  /* negative Y */
        "SpaceBox_front5_posZ_1.bmp",  /* positive Z */
        "SpaceBox_back6_negZ.bmp", /* negative Z */
        true, errorString)) {
        std::cout << "Loaded the tropical sunny day cube map OK" << std::endl;
    }
    else {
        std::cout << "ERROR: Didn't load the tropical sunny day cube map. How sad." << std::endl;
        std::cout << "(because: " << errorString << std::endl;
    }
    
    // ---------------- LOADING TEXTURES ----------------------------------------------

    // ---------------- LUA  ----------------------------------------------

    std::string moveScriptTowardsDestinationFUNCTION =
        "function moveObjectTowardsDestination( objectID, xDest, yDest, zDest )										    \n"	\
        "	isValidObj, xObj, yObj, zObj, vxObj, vyObj, vzObj, moving = getObjectState( objectID )		\n"	\
        "	if isValidObj and xDest ~= nil and yDest ~= nil and zDest ~= nil and moving == 1 then					\n"	\
        "		xDirection = xDest - xObj																		    \n"	\
        "		yDirection = yDest - yObj																		    \n"	\
        "		zDirection = zDest - zObj																			\n"	\
        "		magnitude = (xDirection * xDirection + yDirection * yDirection + zDirection * zDirection)^0.5		\n"	\
        "		if magnitude > " + std::to_string(SMALLEST_DISTANCE) + " then										\n"	\
        "			xStep = xDirection / magnitude / 2																\n"	\
        "			yStep = yDirection / magnitude / 2																\n"	\
        "			zStep = zDirection / magnitude / 2         													    \n"	\
        "																                                    \n"	\
        "			xObj = xObj + xStep																					\n"	\
        "			yObj = yObj + yStep																				    \n"	\
        "			zObj = zObj + zStep																					\n"	\
        "																								\n"	\
        "			setObjectState( objectID, xObj, yObj, zObj, vyObj, vzObj, vzObj, 1 )							\n"	\
        "			return true																						\n"	\
        "		else																								\n"	\
        "		    print(\"Current Object: \", objectID ) 															\n"	\
        "		    print(\"Setting this position: \", xObj, yObj, zObj ) 															\n"	\
        "			setObjectState( objectID, xObj, yObj, zObj, vx, vy, vz, 0 )							\n"	\
        "			return false																					\n"	\
        "		end																									\n"	\
        "	end																										\n"	\
        "end";
    pBrain->RunScriptImmediately(moveScriptTowardsDestinationFUNCTION);

    pBrain->SetObjectVector(g_GraphicScene.map_beholds);

    for (std::map< std::string, cMeshObject*>::iterator itBeholds =
        g_GraphicScene.map_beholds->begin(); itBeholds != g_GraphicScene.map_beholds->end();
        itBeholds++) {
        cMeshObject* currentBehold = itBeholds->second;
        unsigned int currentBeholdID = currentBehold->getID();

        std::string currentBeholdNextTileXScript = "nextTileX" + std::to_string(currentBeholdID) + " = nil";
        std::string currentBeholdNextTileYScript = "nextTileY" + std::to_string(currentBeholdID) + " = nil";
        std::string currentBeholdNextTileZScript = "nextTileZ" + std::to_string(currentBeholdID) + " = nil";
        pBrain->RunScriptImmediately(currentBeholdNextTileXScript);
        pBrain->RunScriptImmediately(currentBeholdNextTileYScript);
        pBrain->RunScriptImmediately(currentBeholdNextTileZScript);

        std::string myScriptText = "setObjectState(" + std::to_string(currentBeholdID) + ", " +
            std::to_string(currentBehold->position.x) + ", " +
            std::to_string(currentBehold->position.y) + ", " +
            std::to_string(currentBehold->position.z) + ", " + 
            " 0, 0, 0 " +
            ")";
        pBrain->RunScriptImmediately(myScriptText);

        pBrain->LoadScript("behold" + std::to_string(currentBeholdID) + "BasicMove", "moveObjectTowardsDestination(" +
            std::to_string(currentBeholdID) + ", " + 
            "nextTileX" + std::to_string(currentBeholdID) + ", " +
            "nextTileY" + std::to_string(currentBeholdID) + ", " +
            "nextTileZ" + std::to_string(currentBeholdID) + ")");
    }
    // ---------------- LUA  ----------------------------------------------

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

        // Making the fire "move"
        for (int i = 0; i < g_GraphicScene.vec_torchFlames.size(); i++) {
            cMeshObject* torchFire = g_GraphicScene.vec_torchFlames[i];
            if (torchFire) {
                torchFire->scaleXYZ.y += RandomFloat(-0.1f, 0.1f);
            }
        }

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

        // --------------- Draw the skybox -----------------------------
        {
            GLint bIsSkyboxObject_UL = glGetUniformLocation(shaderID, "bIsSkyboxObject");
            glUniform1f(bIsSkyboxObject_UL, (GLfloat)GL_TRUE);

            glm::mat4x4 matModel = glm::mat4x4(1.0f);

            // move skybox to the cameras location
            pSkyBox->position = ::g_cameraEye;

            // The scale of this large skybox needs to be smaller than the far plane
            //  of the projection matrix.
            // Maybe make it half the size
            // Here, our far plane is 10000.0f...
            pSkyBox->SetUniformScale(7500.0f);

            // All the drawing code has been moved to the DrawObject function
            DrawObject(pSkyBox,
                matModel,
                shaderID, ::g_pTextureManager,
                pVAOManager, mModel_location, mModelInverseTransform_location);

            // Turn this off
            glUniform1f(bIsSkyboxObject_UL, (GLfloat)GL_FALSE);
        }
        // --------------- Draw the skybox -----------------------------
        
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
        //simView->m_PhysicsSystem.UpdateStep(0.1f);
        for (std::map< std::string, cMeshObject*>::iterator itBeholds =
            g_GraphicScene.map_beholds->begin(); itBeholds != g_GraphicScene.map_beholds->end();
            itBeholds++) {
            cMeshObject* currentBehold = itBeholds->second;
            unsigned int currentBeholdID = currentBehold->getID();

            if (currentBehold->moving == 0 && !currentBehold->dead) {
                glm::vec3 nextPosition = glm::vec3(0.0f);
                if (!calculateNextPosition(currentBehold, nextPosition)) {
                    // Spin and reduce;
                    currentBehold->adjustRoationAngleFromEuler(glm::vec3(0.0f, 0.2f, 0.0f));
                    currentBehold->reduceFromScale(0.99f);
                    if (currentBehold->scaleXYZ.x <= 0.01) {
                        currentBehold->dead = true;
                        std::cout << currentBehold->friendlyName << " is Dead :(" << std::endl;
                    }
                    continue;
                }

                std::string currentBeholdNextTileXScript = 
                    "nextTileX" + std::to_string(currentBeholdID) + " = " + 
                    std::to_string(nextPosition.x);
                std::string currentBeholdNextTileYScript =
                    "nextTileY" + std::to_string(currentBeholdID) + " = " +
                    std::to_string(nextPosition.y);
                std::string currentBeholdNextTileZScript = 
                    "nextTileZ" + std::to_string(currentBeholdID) + "  = " +
                    std::to_string(nextPosition.z);

                pBrain->RunScriptImmediately(currentBeholdNextTileXScript);
                pBrain->RunScriptImmediately(currentBeholdNextTileYScript);
                pBrain->RunScriptImmediately(currentBeholdNextTileZScript);

                // Setting rotation
                if (nextPosition.x < currentBehold->position.x) // Going WEST
                    currentBehold->setRotationFromEuler(glm::vec3(0.0f, NinetyDegrees, 0.0f));

                if (nextPosition.x > currentBehold->position.x) // Going EAST
                    currentBehold->setRotationFromEuler(glm::vec3(0.0f, -NinetyDegrees, 0.0f));

                if (nextPosition.z < currentBehold->position.z) // Going NORTH
                    currentBehold->setRotationFromEuler(glm::vec3(0.0f, 0.0, 0.0f));

                if (nextPosition.z > currentBehold->position.z) // Going SOUTH
                    currentBehold->setRotationFromEuler(glm::vec3(0.0f, NinetyDegrees * 2, 0.0f));

                std::cout << " ------------------------------------------------------ " << std::endl;
                std::cout << currentBehold->friendlyName << " CurrentPosition: (" <<
                    currentBehold->position.x << ", " <<
                    currentBehold->position.y << ", " <<
                    currentBehold->position.z << ")" << std::endl;

                std::cout << currentBehold->friendlyName << " I -> " <<
                    currentBehold->currentI << ", J -> " <<
                    currentBehold->currentJ << std::endl;

                std::cout << currentBehold->friendlyName << " NextPosition: (" <<
                    nextPosition.x << ", " <<
                    nextPosition.y << ", " <<
                    nextPosition.z << ")" << std::endl;

                std::cout << "nextTileX" << currentBeholdID << " changed" << std::endl;
                std::cout << "nextTileY" << currentBeholdID << " changed" << std::endl;
                std::cout << "nextTileZ" << currentBeholdID << " changed" << std::endl;

                std::string myScriptText = "setObjectState(" + std::to_string(currentBeholdID) + ", " +
                    std::to_string(currentBehold->position.x) + ", " + 
                    std::to_string(currentBehold->position.y) + ", " + 
                    std::to_string(currentBehold->position.z) + ", " + 
                    " 0, 0, 0, " + 
                    std::to_string(currentBehold->qRotation.x) + ", " + 
                    std::to_string(currentBehold->qRotation.y) + ", " + 
                    std::to_string(currentBehold->qRotation.z) + ", " + 
                    "1)";
                pBrain->RunScriptImmediately(myScriptText);
                currentBehold->moving = 1;
            }
        }

        // Update will run any Lua script sitting in the "brain"
        pBrain->Update(1);

        if (g_GraphicScene.cameraFollowing && !g_GraphicScene.cameraTransitioning) {
            cMeshObject* currentBehold = itBeholdsToFollow->second;
            g_cameraEye.x = currentBehold->position.x + CAMERA_OFFSET;
            g_cameraEye.y = currentBehold->position.y + CAMERA_OFFSET;
            g_cameraEye.y = currentBehold->position.z + CAMERA_OFFSET;

            g_cameraTarget = currentBehold->position;
        } else if (g_GraphicScene.cameraTransitioning) {
            cMeshObject* currentBehold = itBeholdsToFollow->second;
            float distanceCameraBehold = glm::distance(g_cameraEye, currentBehold->position);
            
            float xDir = currentBehold->position.x - g_cameraEye.x;
            float yDir = currentBehold->position.y - g_cameraEye.y;
            float zDir = currentBehold->position.z - g_cameraEye.z;

            g_cameraTarget = currentBehold->position;

            float mag = pow((xDir * xDir + yDir * yDir + zDir * zDir), 0.5);

            if (distanceCameraBehold > CAMERA_OFFSET * 5) {
                
                float xStep = xDir / mag;
                float yStep = yDir / mag;
                float zStep = zDir / mag;

                g_cameraEye.x += xStep * 15;
                g_cameraEye.y += yStep * 15;
                g_cameraEye.z += zStep * 15;
            }
            else {
                g_GraphicScene.cameraFollowing = true;
                g_GraphicScene.cameraTransitioning = false;
            }
        }
        else {
            g_cameraTarget = glm::vec3(500.f, 0.0, 500.f);
            g_cameraEye = glm::vec3(500.f, 2000.f, 510.f);
        }

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