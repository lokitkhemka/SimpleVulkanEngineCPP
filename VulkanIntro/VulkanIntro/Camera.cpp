#include "Camera.hpp"

#include <cassert>

namespace vlkn {
	void Camera::SetOrthographicProj(float Left, float Right, float Top, float Bottom, float Near, float Far)
	{
		ProjMat = glm::mat4{ 1.f };
		ProjMat[0][0] = 2.f / (Right - Left);
		ProjMat[1][1] = 2.f / (Bottom - Top);
		ProjMat[2][2] = 1.f / (Far - Near);
		ProjMat[3][0] = -(Right + Left) / (Right - Left);
		ProjMat[3][1] = -(Bottom + Top) / (Bottom - Top);
		ProjMat[3][2] = -Near / (Far - Near);
	}

	void Camera::SetPerspectiveProj(float FovY, float AspectR, float Near, float Far)
	{
		assert(glm::abs(AspectR - std::numeric_limits<float>::epsilon()) > 0.0f);
		const float tanHalfFovy = tan(FovY / 2.f);
		ProjMat = glm::mat4{ 0.0f };
		ProjMat[0][0] = 1.f / (AspectR * tanHalfFovy);
		ProjMat[1][1] = 1.f / (tanHalfFovy);
		ProjMat[2][2] = Far / (Far - Near);
		ProjMat[2][3] = 1.0f;
		ProjMat[3][2] = -(Far * Near) / (Far - Near);
	}

}
