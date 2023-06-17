#include "App.hpp"
#include "Camera.hpp"
#include "ShaderSystem.hpp"
#include "KeyboardController.hpp"


#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>


#include <stdexcept>
#include <array>
#include <cassert>
#include <chrono>

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
    Camera camera{};
    camera.SetViewDir(glm::vec3(-1.0f, -2.0f, -2.0f), glm::vec3(0.0f,0.0f, 2.5f));
    
    auto ViewerObject = GameObject::CreateGameObject();
    KeyboardController CameraController{};

    auto CurrentTime = std::chrono::high_resolution_clock::now();

	while (!window.ShouldClose())
	{
		glfwPollEvents();

        auto NewTime = std::chrono::high_resolution_clock::now();
        float FrameTime = std::chrono::duration<float, std::chrono::seconds::period>(NewTime - CurrentTime).count();
        CurrentTime = NewTime;

        //FrameTime = glm::min(FrameTime, MAX_FRAME_TIME);

        CameraController.MoveInPlaneXZ(window.getWindowHandle(), FrameTime, ViewerObject);
        camera.SetViewYXZ(ViewerObject.Transform.Translation, ViewerObject.Transform.Rotation);

        float AspectR = renderer.GetAspectRatio();
        //camera.SetOrthographicProj(-AspectR, AspectR, -1, 1, -1, 1);
        camera.SetPerspectiveProj(glm::radians(50.0f), AspectR, 0.1f, 10.0f);


		if (auto CommandBuffer = renderer.BeginFrame())
		{
			renderer.BeginSwapchainRenderPass(CommandBuffer);
			ShaderSys.RenderGameObjects(CommandBuffer, GameObjects,camera);
			renderer.EndSwapchainRenderPass(CommandBuffer);
			renderer.EndFrame();
		}
	}

	vkDeviceWaitIdle(Device.device());
}

namespace vlkn {
    std::unique_ptr<Model> CreateCubeModel(VulkanDevice& Device, glm::vec3 offset) {
        Model::ModelData Data{};
        Data.vertices = {

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
        for (auto& v : Data.vertices) {
            v.position += offset;
        }
        return std::make_unique<Model>(Device, Data);
    }
}

void vlkn::App::LoadGameObjects()
{
    std::shared_ptr<Model> model = CreateCubeModel(Device, {0.0f, 0.0f, 0.0f});

    auto Cube = GameObject::CreateGameObject();
    Cube.Model = model;

    Cube.Transform.Translation = { 0.0f, 0.0f, 2.5f };
    Cube.Transform.Scale = { 0.5f, 0.5f, 0.5f };
    GameObjects.push_back(std::move(Cube));
}
