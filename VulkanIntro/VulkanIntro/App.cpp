#include "App.hpp"
#include "ShaderSystem.hpp"
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include "Model.hpp"
#include "VulkanDevice.hpp"

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
namespace vlkn {
    std::unique_ptr<Model> createCubeModel(VulkanDevice& Device, glm::vec3 offset) {
        std::vector<Model::Vertex> Vertices{

            // left face (white)
            {{-.5f, -.5f, -.5f}, {.9f, .9f, .9f}},
            {{-.5f, .5f, .5f}, {.9f, .9f, .9f}},
            {{-.5f, -.5f, .5f}, {.9f, .9f, .9f}},
            {{-.5f, -.5f, -.5f}, {.9f, .9f, .9f}},
            {{-.5f, .5f, -.5f}, {.9f, .9f, .9f}},
            {{-.5f, .5f, .5f}, {.9f, .9f, .9f}},

            // right face (yellow)
            {{.5f, -.5f, -.5f}, {.8f, .8f, .1f}},
            {{.5f, .5f, .5f}, {.8f, .8f, .1f}},
            {{.5f, -.5f, .5f}, {.8f, .8f, .1f}},
            {{.5f, -.5f, -.5f}, {.8f, .8f, .1f}},
            {{.5f, .5f, -.5f}, {.8f, .8f, .1f}},
            {{.5f, .5f, .5f}, {.8f, .8f, .1f}},

            // top face (orange, remember y axis points down)
            {{-.5f, -.5f, -.5f}, {.9f, .6f, .1f}},
            {{.5f, -.5f, .5f}, {.9f, .6f, .1f}},
            {{-.5f, -.5f, .5f}, {.9f, .6f, .1f}},
            {{-.5f, -.5f, -.5f}, {.9f, .6f, .1f}},
            {{.5f, -.5f, -.5f}, {.9f, .6f, .1f}},
            {{.5f, -.5f, .5f}, {.9f, .6f, .1f}},

            // bottom face (red)
            {{-.5f, .5f, -.5f}, {.8f, .1f, .1f}},
            {{.5f, .5f, .5f}, {.8f, .1f, .1f}},
            {{-.5f, .5f, .5f}, {.8f, .1f, .1f}},
            {{-.5f, .5f, -.5f}, {.8f, .1f, .1f}},
            {{.5f, .5f, -.5f}, {.8f, .1f, .1f}},
            {{.5f, .5f, .5f}, {.8f, .1f, .1f}},

            // nose face (blue)
            {{-.5f, -.5f, 0.5f}, {.1f, .1f, .8f}},
            {{.5f, .5f, 0.5f}, {.1f, .1f, .8f}},
            {{-.5f, .5f, 0.5f}, {.1f, .1f, .8f}},
            {{-.5f, -.5f, 0.5f}, {.1f, .1f, .8f}},
            {{.5f, -.5f, 0.5f}, {.1f, .1f, .8f}},
            {{.5f, .5f, 0.5f}, {.1f, .1f, .8f}},

            // tail face (green)
            {{-.5f, -.5f, -0.5f}, {.1f, .8f, .1f}},
            {{.5f, .5f, -0.5f}, {.1f, .8f, .1f}},
            {{-.5f, .5f, -0.5f}, {.1f, .8f, .1f}},
            {{-.5f, -.5f, -0.5f}, {.1f, .8f, .1f}},
            {{.5f, -.5f, -0.5f}, {.1f, .8f, .1f}},
            {{.5f, .5f, -0.5f}, {.1f, .8f, .1f}},

        };
        for (auto& v : Vertices) {
            v.position += offset;
        }
        return std::make_unique<Model>(Device, Vertices);
    }
}

void vlkn::App::LoadGameObjects()
{
    std::shared_ptr<Model> model = createCubeModel(Device, {0.0f, 0.0f, 0.0f});

    auto Cube = GameObject::CreateGameObject();
    Cube.Model = model;

    Cube.Transform.Translation = { 0.0f, 0.0f, 0.5f };
    Cube.Transform.Scale = { 0.5f, 0.5f, 0.5f };
    GameObjects.push_back(std::move(Cube));
}
