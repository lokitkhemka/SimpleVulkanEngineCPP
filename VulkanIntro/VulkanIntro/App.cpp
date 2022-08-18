#include "App.hpp"
#include "ShaderSystem.hpp"
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

#include <stdexcept>
#include <array>
#include <cassert>

vlkn::App::App()
{
	LoadGameObjects();
}

vlkn::App::~App()
{
}

void vlkn::App::run()
{
	ShaderSystem ShaderSys{Device, renderer.GetSwapchainRenderPass()};
	while (!window.ShouldClose())
	{
		glfwPollEvents();
		if (auto CommandBuffer = renderer.BeginFrame())
		{
			renderer.BeginSwapchainRenderPass(CommandBuffer);
			ShaderSys.RenderGameObjects(CommandBuffer, GameObjects);
			renderer.EndSwapchainRenderPass(CommandBuffer);
			renderer.EndFrame();
		}
	}

	vkDeviceWaitIdle(Device.device());
}

//void vlkn::App::sierpinski(
//	std::vector<Model::Vertex>& vertices,
//	int depth,
//	glm::vec2 left,
//	glm::vec2 right,
//	glm::vec2 top) {
//	if (depth <= 0) {
//		vertices.push_back({ top });
//		vertices.push_back({ right });
//		vertices.push_back({ left });
//	}
//	else {
//		auto leftTop = 0.5f * (left + top);
//		auto rightTop = 0.5f * (right + top);
//		auto leftRight = 0.5f * (left + right);
//		sierpinski(vertices, depth - 1, left, leftRight, leftTop);
//		sierpinski(vertices, depth - 1, leftRight, right, rightTop);
//		sierpinski(vertices, depth - 1, leftTop, rightTop, top);
//	}
//}

void vlkn::App::LoadGameObjects()
{
	std::vector<Model::Vertex> vertices{
		{{0.0f, -0.5f}, {1.0f, 0.0f, 0.0f}},
	  {{0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}},
	  {{-0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}}
	};

	/*std::vector<Model::Vertex> vertices{};
	sierpinski(vertices, 5, { -0.5f, 0.5f }, { 0.5f, 0.5f }, { 0.0f, -0.5f });*/

	auto model = std::make_shared<Model>(Device, vertices);

	auto Tri = GameObject::CreateGameObject();
	Tri.Model = model;
	Tri.Color = { 0.1f, 0.8f, 0.1f };
	Tri.Transform2D.Translation.x = 0.2f;
	Tri.Transform2D.Scale = { 2.0f, 0.5f };
	Tri.Transform2D.Rotation = 0.25f * glm::two_pi<float>();

	GameObjects.push_back(std::move(Tri));
}
