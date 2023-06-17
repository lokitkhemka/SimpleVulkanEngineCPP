#include "KeyboardController.hpp"

namespace vlkn{
void KeyboardController::MoveInPlaneXZ(GLFWwindow* window, float dt, GameObject& gameObject)
{
	glm::vec3 Rotate{ 0 };

	if (glfwGetKey(window, Keys.LookRight) == GLFW_PRESS) Rotate.y += 1.f;
	if (glfwGetKey(window, Keys.LookLeft) == GLFW_PRESS) Rotate.y -= 1.f;
	if (glfwGetKey(window, Keys.LookUp) == GLFW_PRESS) Rotate.x += 1.f;
	if (glfwGetKey(window, Keys.LookDown) == GLFW_PRESS) Rotate.x -= 1.f;

	if (glm::dot(Rotate,Rotate) > std::numeric_limits<float>::epsilon())
		gameObject.Transform.Rotation += TurnSpeed * dt * glm::normalize(Rotate);

	gameObject.Transform.Rotation.x = glm::clamp(gameObject.Transform.Rotation.x, -1.5f, 1.5f);
	gameObject.Transform.Rotation.y = glm::mod(gameObject.Transform.Rotation.y, glm::two_pi<float>());

	float Yaw = gameObject.Transform.Rotation.y;
	const glm::vec3 ForwardDir{ sin(Yaw), 0, cos(Yaw) };
	const glm::vec3 RightDir{ ForwardDir.z, 0.0f, -ForwardDir.x };
	const glm::vec3 UpDir{ 0.0f, -1.0f, 0.0f };

	glm::vec3 MoveDir{ 0.0f };

	if (glfwGetKey(window, Keys.MoveForward) == GLFW_PRESS) MoveDir += ForwardDir;
	if (glfwGetKey(window, Keys.MoveBackward) == GLFW_PRESS) MoveDir -= ForwardDir;
	if (glfwGetKey(window, Keys.MoveRight) == GLFW_PRESS) MoveDir += RightDir;
	if (glfwGetKey(window, Keys.MoveLeft) == GLFW_PRESS) MoveDir -= RightDir;
	if (glfwGetKey(window, Keys.MoveUp) == GLFW_PRESS) MoveDir += UpDir;
	if (glfwGetKey(window, Keys.MoveDown) == GLFW_PRESS) MoveDir -= UpDir;

	if (glm::dot(MoveDir, MoveDir) > std::numeric_limits<float>::epsilon())
		gameObject.Transform.Translation += MoveSpeed * dt * glm::normalize(MoveDir);
}
}
