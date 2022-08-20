#pragma once

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

namespace vlkn {

	class Camera {

	public:
		void SetOrthographicProj(float Left, float Right, float Top, float Bottom, float Near, float Far);

		void SetPerspectiveProj(float FovY, float AspectR, float Near, float Far);
		const glm::mat4& GetProjMat() const { return ProjMat; }

	private:
		glm::mat4 ProjMat{ 1.0f };
	};


}