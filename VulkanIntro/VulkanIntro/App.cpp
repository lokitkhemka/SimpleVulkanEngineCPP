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



    void vlkn::App::LoadGameObjects()
    {
        std::shared_ptr<Model> model = Model::CreateModelFromObj(Device, "./models/flat_vase.obj");

        auto GameObj = GameObject::CreateGameObject();
        GameObj.Model = model;

        GameObj.Transform.Translation = { 0.0f, 0.5f, 2.5f };
        GameObj.Transform.Scale = { 0.5f, 0.5f, 0.5f };
        GameObjects.push_back(std::move(GameObj));
    }
}
