#pragma once

#include "GL.h"

#include <string>

namespace gdp
{
	struct Shader {
		std::string filename;
		std::string source;
		bool bIsCompiled;
		GLuint id;
	};

	class ShaderProgram {
	public:
		ShaderProgram(const char* vertexFilepath, const char* fragmentFilepath);
		~ShaderProgram();

		std::string name;
		Shader vertexShader;
		Shader fragmentShader;
		GLuint id;
	};
}