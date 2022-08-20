#pragma once

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

namespace vlkn {

	class Camera {

	public:
		void SetOrthographicProj(float Left, float Right, float Top, float Bottom, float Near, float Far);

		void SetPerspectiveProj(float FovY, float AspectR, float Near, float Far);

		void SetViewDir(glm::vec3 CamPos, glm::vec3 CamDir, glm::vec3 CamUp = glm::vec3{ 0.0f, -1.0f, 0.0f });
		void SetViewTarget(glm::vec3 CamPos, glm::vec3 TargetDir, glm::vec3 CamUp = glm::vec3{ 0.0f, -1.0f, 0.0f });
		void SetViewYXZ(glm::vec3 Pos, glm::vec3 Rotation);

		const glm::mat4& GetProjMat() const { return ProjMat; }
		const glm::mat4& GetViewMat() const { return ViewMat; }

	private:
		glm::mat4 ProjMat{ 1.0f };
		glm::mat4 ViewMat{ 1.0f };
	};


}