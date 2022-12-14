
#include "globalOpenGL.h"   // for the GL calls.
#include <glm/glm.hpp>
#include <glm/vec3.hpp> // glm::vec3        (x,y,z)
#include <glm/vec4.hpp> // glm::vec4        (x,y,z,w)
#include <glm/mat4x4.hpp> // glm::mat4
// glm::translate, glm::rotate, glm::scale, glm::perspective
#include <glm/gtc/matrix_transform.hpp> 
#include <glm/gtc/type_ptr.hpp> // glm::value_ptr

#include "cMeshObject.h"
#include "cBasicTextureManager/cBasicTextureManager.h"
#include "cVAOManager/cVAOManager.h"
#include "cVAOManager/sModelDrawInfo.h"

//#if DEBUG
#include <iostream>
//#endif // DEBUG

void DrawObject(cMeshObject* pCurrentMeshObject,
                glm::mat4x4 mat_PARENT_Model,               // The "parent's" model matrix
                GLuint shaderID,                            // ID for the current shader
                cBasicTextureManager* pTextureManager,
                cVAOManager* pVAOManager,
                GLint mModel_location,                      // Uniform location of mModel matrix
                GLint mModelInverseTransform_location)      /* Uniform location of mView location*/ {

    // Don't draw any "back facing" triangles
    glCullFace(GL_BACK);
    glEnable(GL_CULL_FACE);

    // Turn on depth buffer test at draw time
    glEnable(GL_DEPTH_TEST);

    // *********************************************************
    // HACK: Check if this is a flame object
    GLint bIsFlameObject_UniformLocation = glGetUniformLocation(shaderID, "bIsFlameObject");
    // HACK: I'm checking to see if the texture matches the flame object
    if (pCurrentMeshObject->textures[0] == "fire-torch-texture.bmp")
    {
        glUniform1f(bIsFlameObject_UniformLocation, (GLfloat)GL_TRUE);
        //        glDisable(GL_DEPTH_TEST);
        //        glDepthFunc(GL_NEVER); // We'll talk about this when we talk about the stencil buffer
        glDepthMask(GL_FALSE);      // DON'T write to the depth buffer
    }
    else
    {
        glUniform1f(bIsFlameObject_UniformLocation, (GLfloat)GL_FALSE);
        //        glDepthFunc(GL_LESS); // We'll talk about this when we talk about the stencil buffer
        glDepthMask(GL_TRUE);      // DON'T write to the depth buffer
    }
    // *********************************************************

    glm::mat4x4 matModel = mat_PARENT_Model;  // identity matrix;

    // Move the object 
    glm::mat4 matTranslation = glm::translate(glm::mat4(1.0f),
                                              pCurrentMeshObject->position);
    
    glm::mat4 matQRotation = glm::mat4(pCurrentMeshObject->qRotation);

    // Scale the object
    glm::mat4 matScale = glm::scale(glm::mat4(1.0f),
                                    glm::vec3(pCurrentMeshObject->scaleXYZ.x,
                                              pCurrentMeshObject->scaleXYZ.y,
                                              pCurrentMeshObject->scaleXYZ.z));

    // Applying all these transformations to the MODEL 
    // (or "world" matrix)
    matModel = matModel * matTranslation;

    matModel = matModel * matQRotation;

    matModel = matModel * matScale;

    glUniformMatrix4fv(mModel_location, 1, GL_FALSE, glm::value_ptr(matModel));
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
    GLint RGBA_Colour_ULocID = glGetUniformLocation(shaderID, "RGBA_Colour");

    // Turn on alpha transparency for everything
    // Maybe: if ( alpha < 1.0 ) ...
    glEnable(GL_BLEND);
    // Basic alpha transparency:
    // 0.5 --> 0.5 what was already on the colour buffer 
    //       + 0.5 of this object being drawn
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glUniform4f(RGBA_Colour_ULocID,
                pCurrentMeshObject->RGBA_colour.r,
                pCurrentMeshObject->RGBA_colour.g,
                pCurrentMeshObject->RGBA_colour.b,
                pCurrentMeshObject->RGBA_colour.w);     // Transparency


    GLint bUseRGBA_Colour_ULocID = glGetUniformLocation(shaderID, "bUseRGBA_Colour");

    if (pCurrentMeshObject->bUse_RGBA_colour) {
        glUniform1f(bUseRGBA_Colour_ULocID, (GLfloat)GL_TRUE);
    }
    else {
        glUniform1f(bUseRGBA_Colour_ULocID, (GLfloat)GL_FALSE);
    }

    // Copy specular object colour and power. 
    GLint specularColour_ULocID = glGetUniformLocation(shaderID, "specularColour");

    glUniform4f(specularColour_ULocID,
                pCurrentMeshObject->specular_colour_and_power.r,
                pCurrentMeshObject->specular_colour_and_power.g,
                pCurrentMeshObject->specular_colour_and_power.b,
                pCurrentMeshObject->specular_colour_and_power.w);

    //uniform bool bDoNotLight;	
    GLint bDoNotLight_Colour_ULocID = glGetUniformLocation(shaderID, "bDoNotLight");

    if (pCurrentMeshObject->bDoNotLight) {
        glUniform1f(bDoNotLight_Colour_ULocID, (GLfloat)GL_TRUE);
    }
    else {
        glUniform1f(bDoNotLight_Colour_ULocID, (GLfloat)GL_FALSE);
    }

    // Set up the textures on this model
    std::string texture0Name = pCurrentMeshObject->textures[0];
    std::string texture1Name = pCurrentMeshObject->textures[1];

    GLuint texture00Number = pTextureManager->getTextureIDFromName(texture0Name);

    //// Choose the texture Unit I want
    GLuint texture00Unit = 0;			// Texture unit go from 0 to 79
    glActiveTexture(texture00Unit + GL_TEXTURE0);	// GL_TEXTURE0 = 33984

    // Pick the texture 
    // 1. make it "active" (binding)
    // 2. Attatches it the current ACTIVE TEXTURE UNIT
    glBindTexture(GL_TEXTURE_2D, texture00Number);

    GLint texture0_UL = glGetUniformLocation(shaderID, "texture0");
    glUniform1i(texture0_UL, texture00Unit);

    // Same for texture #1
    GLuint texture01Number = pTextureManager->getTextureIDFromName(texture1Name);
    GLuint texture01Unit = 1;			// Texture unit go from 0 to 79
    glActiveTexture(texture01Unit + GL_TEXTURE0);	// GL_TEXTURE0 = 33984
    glBindTexture(GL_TEXTURE_2D, texture01Number);
    GLint texture1_UL = glGetUniformLocation(shaderID, "texture1");
    glUniform1i(texture1_UL, texture01Unit);

    // Do that for the other 6 textures... FUN!

    //// Same for texture #7
    //std::string texture7Name = pCurrentMeshObject->textures[7];
    //GLuint texture07Number = pTextureManager->getTextureIDFromName(texture7Name);
    //GLuint texture07Unit = 7;			// Texture unit go from 0 to 79
    //glActiveTexture(texture07Unit + GL_TEXTURE0);	// GL_TEXTURE0 = 33984
    //glBindTexture(GL_TEXTURE_2D, texture07Number);
    //GLint texture7_UL = glGetUniformLocation(shaderID, "texture7");
    //glUniform1i(texture7_UL, texture07Unit);

    // uniform vec4 texRatio_0_3;
    GLint texRatio_0_3 = glGetUniformLocation(shaderID, "texRatio_0_3");
    glUniform4f(texRatio_0_3,
                pCurrentMeshObject->textureRatios[0],
                pCurrentMeshObject->textureRatios[1],
                pCurrentMeshObject->textureRatios[2],
                pCurrentMeshObject->textureRatios[3]);

    // The cube map textures
    GLuint cubeMapTextureNumber = pTextureManager->getTextureIDFromName("SpaceBox");
    GLuint texture30Unit = 30;			// Texture unit go from 0 to 79
    glActiveTexture(texture30Unit + GL_TEXTURE0);	// GL_TEXTURE0 = 33984
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMapTextureNumber);
    GLint skyboxTexture_UL = glGetUniformLocation(shaderID, "skyboxTexture");
    glUniform1i(skyboxTexture_UL, texture30Unit);

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

    // Draw all the children
    for (std::vector< cMeshObject* >::iterator itCurrentMesh = pCurrentMeshObject->vecChildMeshes.begin();
         itCurrentMesh != pCurrentMeshObject->vecChildMeshes.end();
         itCurrentMesh++) {
        cMeshObject* pCurrentCHILDMeshObject = *itCurrentMesh;        // * is the iterator access thing

       // All the drawing code has been moved to the DrawObject function
        DrawObject(pCurrentCHILDMeshObject, 
                   matModel,                // The FINAL matrix that the parent use
                   shaderID, pTextureManager,
                   pVAOManager, mModel_location, mModelInverseTransform_location);
    }

    return;
}
