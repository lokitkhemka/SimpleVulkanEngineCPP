#include "App.hpp"
#include "Camera.hpp"
#include "ShaderSystem.hpp"
#include "KeyboardController.hpp"
#include "VulkanBufferObjects.hpp"


#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>


#include <stdexcept>
#include <array>
#include <cassert>
#include <chrono>

namespace vlkn {
    struct GlobalUBO {
        glm::mat4 ProjectionView{ 1.0f };
        glm::vec3 LightDirection = glm::normalize(glm::vec3{1.0f, -3.0f, -1.0f});
    };
}

vlkn::App::App()
{
	LoadGameObjects();
}

vlkn::App::~App()
{
}

void vlkn::App::run()
{
    VulkanBufferObjects GlobalUBOBuffer{
    Device, sizeof(vlkn::GlobalUBO), vlkn::Swapchain::MAX_FRAMES_IN_FLIGHT,
    VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
    VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT,
    Device.properties.limits.minUniformBufferOffsetAlignment
    };
    GlobalUBOBuffer.Map();


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
            int FrameIndex = renderer.GetFrameIndex();
            //Update Buffers
            GlobalUBO ubo{};
            ubo.ProjectionView = camera.GetProjMat() * camera.GetViewMat();
            GlobalUBOBuffer.WriteToIndex(&ubo, FrameIndex);
            GlobalUBOBuffer.FlushIndex(FrameIndex);
            //Render
			renderer.BeginSwapchainRenderPass(CommandBuffer);
			ShaderSys.RenderGameObjects(CommandBuffer, GameObjects,camera);
			renderer.EndSwapchainRenderPass(CommandBuffer);
			renderer.EndFrame();
		}
	}

	vkDeviceWaitIdle(Device.device());
}

namespace vlkn {



    void App::LoadGameObjects()
    {
        std::shared_ptr<Model> model = Model::CreateModelFromObj(Device, "./models/smooth_vase.obj");

        auto GameObj = GameObject::CreateGameObject();
        GameObj.Model = model;

        GameObj.Transform.Translation = { 0.0f, 0.5f, 2.5f };
        GameObj.Transform.Scale = { 0.5f, 0.5f, 0.5f };
        GameObjects.push_back(std::move(GameObj));
    }
}
