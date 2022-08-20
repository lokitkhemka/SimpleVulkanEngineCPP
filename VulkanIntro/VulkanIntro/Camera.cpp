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

	void Camera::SetViewDir(glm::vec3 CamPos, glm::vec3 CamDir, glm::vec3 CamUp)
	{
		const glm::vec3 w{ glm::normalize(CamDir) };
		const glm::vec3 u{ glm::normalize(glm::cross(w, CamUp)) };
		const glm::vec3 v{ glm::cross(w, u) };

		ViewMat = glm::mat4{ 1.f };
		ViewMat[0][0] = u.x;
		ViewMat[1][0] = u.y;
		ViewMat[2][0] = u.z;
		ViewMat[0][1] = v.x;
		ViewMat[1][1] = v.y;
		ViewMat[2][1] = v.z;
		ViewMat[0][2] = w.x;
		ViewMat[1][2] = w.y;
		ViewMat[2][2] = w.z;
		ViewMat[3][0] = -glm::dot(u, CamPos);
		ViewMat[3][1] = -glm::dot(v, CamPos);
		ViewMat[3][2] = -glm::dot(w, CamPos);
	}

	void Camera::SetViewTarget(glm::vec3 CamPos, glm::vec3 TargetDir, glm::vec3 CamUp)
	{
		SetViewDir(CamPos, TargetDir - CamPos, CamUp);
	}
	
	void Camera::SetViewYXZ(glm::vec3 Pos, glm::vec3 Rotation)
	{
		const float c3 = glm::cos(Rotation.z);
		const float s3 = glm::sin(Rotation.z);
		const float c2 = glm::cos(Rotation.x);
		const float s2 = glm::sin(Rotation.x);
		const float c1 = glm::cos(Rotation.y);
		const float s1 = glm::sin(Rotation.y);
		const glm::vec3 u{ (c1 * c3 + s1 * s2 * s3), (c2 * s3), (c1 * s2 * s3 - c3 * s1) };
		const glm::vec3 v{ (c3 * s1 * s2 - c1 * s3), (c2 * c3), (c1 * c3 * s2 + s1 * s3) };
		const glm::vec3 w{ (c2 * s1), (-s2), (c1 * c2) };
		ViewMat = glm::mat4{ 1.f };
		ViewMat[0][0] = u.x;
		ViewMat[1][0] = u.y;
		ViewMat[2][0] = u.z;
		ViewMat[0][1] = v.x;
		ViewMat[1][1] = v.y;
		ViewMat[2][1] = v.z;
		ViewMat[0][2] = w.x;
		ViewMat[1][2] = w.y;
		ViewMat[2][2] = w.z;
		ViewMat[3][0] = -glm::dot(u, Pos);
		ViewMat[3][1] = -glm::dot(v, Pos);
		ViewMat[3][2] = -glm::dot(w, Pos);
	}

}
