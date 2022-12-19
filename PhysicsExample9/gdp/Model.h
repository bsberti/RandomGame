#pragma once

#include "GL.h"

#include <vector>

namespace gdp
{
	class Model {
	public:
		Model(const char* filepath);
		Model(const std::vector<glm::vec3>& vertices, const std::vector<int>& triangles);
		~Model();

		GLuint Vbo;
		GLuint VertexBufferId;
		GLuint IndexBufferId;

		std::vector<glm::vec3> vertices;
		std::vector<int> triangles;	// 1,2,3

		unsigned int NumTriangles;
	};
}