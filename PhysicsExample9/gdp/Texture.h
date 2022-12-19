#pragma once

#include "GL.h"

namespace gdp
{
	class Texture {
	public:
		Texture(const char* filepath);
		~Texture();

		GLenum glEnum;
		GLuint id;
	};
}