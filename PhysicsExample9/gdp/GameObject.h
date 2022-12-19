#pragma once

#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>

#include "MeshRenderer.h"

namespace gdp
{
	class GameObject {
	public:
		GameObject()
			: Position(0.0f)
			, Scale(1.0f)
			, Rotation(glm::quat(1.0f, 0.0f, 0.0f, 0.0f))
			, Renderer()
			, Enabled(true)
		{ }

		glm::vec3 Position;
		glm::vec3 Scale;
		glm::quat Rotation;
		MeshRenderer Renderer;
		bool Enabled;
	};
}