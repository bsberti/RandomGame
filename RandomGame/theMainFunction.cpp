
//#include <glad/glad.h>
//#define GLFW_INCLUDE_NONE
//#include <GLFW/glfw3.h>
#include "globalOpenGL.h"

#include <glm/glm.hpp>
#include <glm/vec3.hpp> // glm::vec3        (x,y,z)
#include <glm/vec4.hpp> // glm::vec4        (x,y,z,w)
#include <glm/mat4x4.hpp> // glm::mat4
#include <glm/gtc/matrix_transform.hpp> 
#include <glm/gtc/type_ptr.hpp> // glm::value_ptr
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <fstream>  // File streaming thing (like cin, etc.)
#include <sstream>  // "string builder" type thing

// Some STL (Standard Template Library) things
#include <vector>           // aka a "smart array"
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

glm::vec3 g_cameraEye = glm::vec3(-75.0, 75.0, -500.0f);
glm::vec3 g_cameraTarget = glm::vec3(0.0f, 0.0f, 0.0f);

cVAOManager* pVAOManager;
GraphicScene g_GraphicScene;
ParticleSystem g_ParticleSystem;
cRandomUI gameUi;
SimulationView* simView = new SimulationView();
GLFWwindow* window;

int const NUMBER_ROBOTS = 10;

// Call back signatures here
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);

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
    std::string PLYFileNameToLoad;     // = "assets/models/MOTO/Blender (load from OBJ export) - only Moto_xyz_n_rgba_uv.ply";
    std::string friendlyName;   // = "MOTO";

    bool bKeepReadingFile = true;

    const unsigned int BUFFER_SIZE = 1000;  // 1000 characters
    char textBuffer[BUFFER_SIZE];       // Allocate AND clear (that's the {0} part)
    // Clear that array to all zeros
    memset(textBuffer, 0, BUFFER_SIZE);

    // Or if it's integers, you can can do this short cut:
    // char textBuffer[BUFFER_SIZE] = { 0 };       // Allocate AND clear (that's the {0} part)

    while (bKeepReadingFile) {
        // Reads the entire line into the buffer (including any white space)
        modelTypeFile.getline(textBuffer, BUFFER_SIZE);

        PLYFileNameToLoad.clear();  // Erase whatever is already there (from before)
        PLYFileNameToLoad.append(textBuffer);

        // Is this the end of the file (have I read "EOF" yet?)?
        if (PLYFileNameToLoad == "EOF")
        {
            // All done
            bKeepReadingFile = false;
            // Skip to the end of the while loop
            continue;
        }

        // Load the "friendly name" line also
        memset(textBuffer, 0, BUFFER_SIZE);
        modelTypeFile.getline(textBuffer, BUFFER_SIZE);
        friendlyName.clear();
        friendlyName.append(textBuffer);

        sModelDrawInfo motoDrawInfo;

        c3DModelFileLoader fileLoader;
        //if (LoadThePLYFile(PLYFileNameToLoad, motoDrawInfo))
        std::string errorText = "";
        if (fileLoader.LoadPLYFile_Format_XYZ_N_RGBA_UV(PLYFileNameToLoad, motoDrawInfo, errorText)) {
            std::cout << "Loaded the file OK" << std::endl;
        }
        else {
            std::cout << errorText;
        }

        if (pVAOManager->LoadModelIntoVAO(friendlyName, motoDrawInfo, shaderID)) {
            std::cout << "Loaded the " << friendlyName << " model" << std::endl;
        }
    }

    return true;
}

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

    ::g_pTheLightManager->CreateBasicDirecLight(shaderID, glm::vec4(-75.0f, 400.0f, -75.0f, 0.0f));
    //::g_pTheLightManager->CreateBasicSpotLight(shaderID, glm::vec4(103.0f, 7.0f, 67.0f, 0.0f));
}

void creatingModelsPhysicsMedia() {

    sModelDrawInfo drawingInformation;    
    pVAOManager->FindDrawInfoByModelName("Terrain_Florest", drawingInformation);
    g_GraphicScene.CreateGameObjectByType("Terrain_Florest", glm::vec3(0.0f, 0.0f, 0.0f), drawingInformation);
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

int main(int argc, char* argv[]) {
    std::cout << "starting up..." << std::endl;

    GLuint vertex_buffer = 0;
    GLuint shaderID = 0;
    GLint vpos_location = 0;
    GLint vcol_location = 0;

    int updateCount = 0;

    pVAOManager = new cVAOManager();

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

    gameUi.iniciatingUI();

    // NOTE: OpenGL error checks have been omitted for brevity
    //int x = 9;              // Location 7363
    //int* px = &x;           // px = 7363
    //int* pX1 = &x;          // px1 = 7363
    //void* pZ = &x;         
    //pZ = RandomFloat;

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
    
    if (!LoadModelTypesIntoVAO("assets/PLYFilesToLoadIntoVAO.txt", pVAOManager, shaderID)) {
        std::cout << "Error: Unable to load list of models to load into VAO file" << std::endl;
        // Do we exit here? 
        // (How do we clean up stuff we've made, etc.)
    }

    // Load the models
    creatingModelsPhysicsMedia();

    cMeshObject* terrain = g_GraphicScene.vec_pMeshObjects[g_GraphicScene.vec_pMeshObjects.size() - 1];

    sModelDrawInfo drawingInformation;
    pVAOManager->FindDrawInfoByModelName("Terrain_Florest", drawingInformation);
    simView->LoadStaticModelToOurAABBEnvironment(drawingInformation, terrain->position);
    
    Ball* mainChar = simView->CreateBall(glm::vec3(10.f, 50.f, 10.f), 1.f);
    mainChar->gameObject->bDoNotLight = false;
    g_GraphicScene.vec_pMeshObjects.push_back(mainChar->gameObject);

    debugLightSpheres();

    GLint mvp_location = glGetUniformLocation(shaderID, "MVP");                 // program
    GLint mModel_location = glGetUniformLocation(shaderID, "mModel");
    GLint mView_location = glGetUniformLocation(shaderID, "mView");
    GLint mProjection_location = glGetUniformLocation(shaderID, "mProjection");
    // Need this for lighting
    GLint mModelInverseTransform_location = glGetUniformLocation(shaderID, "mModelInverseTranspose");

    while (!glfwWindowShouldClose(window)) {
        ::g_pTheLightManager->CopyLightInformationToShader(shaderID);

        // Point the spotlight at the first Object Drawed
        cMeshObject pFirstObject = *g_GraphicScene.vec_pMeshObjects[0];
        glm::vec3 LightToSubRay = pFirstObject.position - glm::vec3(::g_pTheLightManager->vecTheLights[0].position);

        // Normalizing is also just divide by the length of the ray
        // LightToSubRay /= glm::length(LightToSubRay);
        LightToSubRay = glm::normalize(LightToSubRay);

        //::g_pTheLightManager->vecTheLights[0].direction = glm::vec4(LightToSubRay, 1.0f);

        DrawConcentricDebugLightObjects(gameUi.listbox_lights_current);

        float ratio;
        int width, height;
        //mat4x4 m, p, mvp;
        glm::mat4x4 matModel;
        glm::mat4x4 matProjection;
        glm::mat4x4 matView;

        //glm::mat4x4 mvp;            // Model-View-Projection

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
            0.6f,       // Field of view (in degress, more or less 180)
            ratio,
            0.1f,       // Near plane (make this as LARGE as possible)
            10000.0f);   // Far plane (make this as SMALL as possible)
        // 6-8 digits of precision        

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

            // Don't draw any "back facing" triangles
            glCullFace(GL_BACK);

            // Turn on depth buffer test at draw time
            glEnable(GL_DEPTH_TEST);

            // Make an "identity matrix"
            matModel = glm::mat4x4(1.0f);  // identity matrix

            // Move the object 
            glm::mat4 matTranslation = glm::translate(glm::mat4(1.0f),
                pCurrentMeshObject->position);

            // Rotate the object
            glm::mat4 matRoationZ = glm::rotate(glm::mat4(1.0f),
                pCurrentMeshObject->rotation.z,                // Angle to rotate
                glm::vec3(0.0f, 0.0f, 1.0f));       // Axis to rotate around

            glm::mat4 matRoationY = glm::rotate(glm::mat4(1.0f),
                pCurrentMeshObject->rotation.y,                // Angle to rotate
                glm::vec3(0.0f, 1.0f, 0.0f));       // Axis to rotate around

            glm::mat4 matRoationX = glm::rotate(glm::mat4(1.0f),
                pCurrentMeshObject->rotation.x,                // Angle to rotate
                glm::vec3(1.0f, 0.0f, 0.0f));       // Axis to rotate around

            // Scale the object
            float uniformScale = pCurrentMeshObject->scale;
            glm::mat4 matScale = glm::scale(glm::mat4(1.0f),
                glm::vec3(uniformScale, uniformScale, uniformScale));

            // Applying all these transformations to the MODEL 
            // (or "world" matrix)
            matModel = matModel * matTranslation;

            matModel = matModel * matRoationX;
            matModel = matModel * matRoationY;
            matModel = matModel * matRoationZ;

            matModel = matModel * matScale;

            glUniformMatrix4fv(mModel_location, 1, GL_FALSE, glm::value_ptr(matModel));
            glUniformMatrix4fv(mView_location, 1, GL_FALSE, glm::value_ptr(matView));
            glUniformMatrix4fv(mProjection_location, 1, GL_FALSE, glm::value_ptr(matProjection));

            // Inverse transpose of a 4x4 matrix removes the right column and lower row
            // Leaving only the rotation (the upper left 3x3 matrix values)
            glm::mat4 mModelInverseTransform = glm::inverse(glm::transpose(matModel));
            glUniformMatrix4fv(mModelInverseTransform_location, 1, GL_FALSE, glm::value_ptr(mModelInverseTransform));

            // Wireframe
            if (pCurrentMeshObject->isWireframe) {
                glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);      // GL_POINT, GL_LINE, GL_FILL
            }
            else {
                glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            }

            // Setting the colour in the shader
            // uniform vec4 RGBA_Colour;

            GLint RGBA_Colour_ULocID = glGetUniformLocation(shaderID, "RGBA_Colour");

            glUniform4f(RGBA_Colour_ULocID,
                pCurrentMeshObject->RGBA_colour.r,
                pCurrentMeshObject->RGBA_colour.g,
                pCurrentMeshObject->RGBA_colour.b,
                pCurrentMeshObject->RGBA_colour.w);


            GLint bUseRGBA_Colour_ULocID = glGetUniformLocation(shaderID, "bUseRGBA_Colour");

            if (pCurrentMeshObject->bUse_RGBA_colour) {
                glUniform1f(bUseRGBA_Colour_ULocID, (GLfloat)GL_TRUE);
            }
            else {
                glUniform1f(bUseRGBA_Colour_ULocID, (GLfloat)GL_FALSE);
            }

            //uniform bool bDoNotLight;	
            GLint bDoNotLight_Colour_ULocID = glGetUniformLocation(shaderID, "bDoNotLight");

            if (pCurrentMeshObject->bDoNotLight) {
                glUniform1f(bDoNotLight_Colour_ULocID, (GLfloat)GL_TRUE);
            }
            else {
                glUniform1f(bDoNotLight_Colour_ULocID, (GLfloat)GL_FALSE);
            }

            // Choose the VAO that has the model we want to draw...
            sModelDrawInfo drawingInformation;
            if (pVAOManager->FindDrawInfoByModelName(pCurrentMeshObject->meshName, drawingInformation)) {
                glBindVertexArray(drawingInformation.VAO_ID);

                glDrawElements(GL_TRIANGLES,
                    drawingInformation.numberOfIndices,
                    GL_UNSIGNED_INT,
                    (void*)0);

                glBindVertexArray(0);
            }
            else {
                // Didn't find that model
                std::cout << "Error: didn't find model to draw." << std::endl;
            }
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

        gameUi.render(g_GraphicScene, g_pTheLightManager->vecTheLights);

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

        mainChar->gameObject->position = mainChar->physicsObject->GetPosition().GetGLM();
        g_cameraTarget = glm::vec3(mainChar->gameObject->position.x, 
            mainChar->gameObject->position.y, 
            mainChar->gameObject->position.z);

        g_cameraEye = glm::vec3(mainChar->gameObject->position.x + 0.f,
            mainChar->gameObject->position.y + 40.f,
            mainChar->gameObject->position.z + -50.f);

        glfwSetWindowTitle(window, ssTitle.str().c_str());
    }

    // Get rid of stuff
    delete pTheShaderManager;
    delete ::g_pTheLightManager;

    glfwDestroyWindow(window);

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