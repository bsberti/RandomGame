#include "PhysicsDebugRenderer.h"

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/matrix_decompose.hpp>

#include <GL/glew.h>

#include "gdp/Engine.h"

struct PhysicsDebugRendererShader {
    GLuint Color;
    GLuint ModelMatrix;
    GLuint ProjectionMatrix;
    GLuint ViewMatrix;
} debugShader;

PhysicsDebugRenderer::PhysicsDebugRenderer()
{
    gdp::GDP_LoadModel(m_BoundingBoxModelId, "assets/models/bounding_box.obj");
    gdp::GDP_LoadModel(m_BoundingSphereModelId, "assets/models/bounding_sphere.obj");

    unsigned int shaderId;
    gdp::GDP_CreateShaderProgramFromFiles(shaderId, m_DebugShaderProgramId,
        "assets/shaders/PhysicsDebugShader.vertex.glsl",
        "assets/shaders/PhysicsDebugShader.fragment.glsl");

    glUseProgram(m_DebugShaderProgramId);
    debugShader.ModelMatrix = glGetUniformLocation(m_DebugShaderProgramId, "ModelMatrix");
    debugShader.ViewMatrix = glGetUniformLocation(m_DebugShaderProgramId, "ViewMatrix");
    debugShader.ProjectionMatrix = glGetUniformLocation(m_DebugShaderProgramId, "ProjectionMatrix");
    debugShader.Color = glGetUniformLocation(m_DebugShaderProgramId, "Color");

    int breakpointcheckvalues = 0;
}

PhysicsDebugRenderer::~PhysicsDebugRenderer()
{ 

}

void PhysicsDebugRenderer::Render()
{
    glClear(GL_DEPTH_BUFFER_BIT);

    glm::mat4 projectionMatrix;
    glm::mat4 viewMatrix;

    glm::vec3 eye = glm::vec3(0.0f, 32.0f, -48.0f);

    projectionMatrix = glm::perspective(glm::radians(45.0f), ((GLfloat)1200) / ((GLfloat)800), 0.1f, 5000.0f);
    viewMatrix = glm::lookAt(eye, glm::vec3(0,0,0), glm::vec3(0.0f, 1.0f, 0.0f));

    glUniformMatrix4fv(debugShader.ProjectionMatrix, 1, GL_FALSE, glm::value_ptr(projectionMatrix));
    glUniformMatrix4fv(debugShader.ViewMatrix, 1, GL_FALSE, glm::value_ptr(viewMatrix));

    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	for (int i = 0; i < m_PhysicsObjects.size(); i++) {
        PhysicsObject* physicsObject = m_PhysicsObjects[i];

		// Model Matrix
		// debug model -> Cube
		// Render
		// First try, we will try to use the current shader
		// Otherwise we will have to write a custom one.
        Vector3 physicsPosition = physicsObject->GetPosition();
        glm::vec3 glmPosition(physicsPosition.x, physicsPosition.y, physicsPosition.z);

        Vector3 scale = physicsObject->pBoundingBox->halfExtents;

        glm::mat4 TranslationMatrix = glm::translate(glm::mat4(1.0), glmPosition);
        glm::mat4 RotationMatrix = glm::mat4_cast(glm::quat(1, 0, 0, 0));
        glm::mat4 ScaleMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(scale.x, scale.y, scale.z));
        glm::mat4 ModelMatrix = TranslationMatrix * RotationMatrix * ScaleMatrix;

        glUniform3fv(debugShader.Color, 1, glm::value_ptr(glm::vec3(0, 0, 0)));

        glUniformMatrix4fv(debugShader.ModelMatrix, 1, GL_FALSE, glm::value_ptr(ModelMatrix));
        //glUniformMatrix4fv(gSimpleShader.RotationMatrix, 1, GL_FALSE, glm::value_ptr(RotationMatrix));

        unsigned int numTriangles;
        unsigned int vbo;
        std::vector<glm::vec3> vertices;
        std::vector<int> triangles;
        gdp::GDP_GetModelData(m_BoundingBoxModelId, vertices, triangles, numTriangles, vbo);
        glBindVertexArray(vbo);
        glDrawElements(GL_TRIANGLES, numTriangles * 3, GL_UNSIGNED_INT, (GLvoid*)0);
	}

    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}


void PhysicsDebugRenderer::AddPhysicsObject(PhysicsObject* object)
{
	// TODO: Check if the object is already added to the vector.

	m_PhysicsObjects.push_back(object);
}
