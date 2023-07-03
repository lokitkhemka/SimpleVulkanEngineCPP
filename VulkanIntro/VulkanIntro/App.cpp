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
#include <numeric>

namespace vlkn {
    struct GlobalUBO {
        glm::mat4 ProjectionView{ 1.0f };
        glm::vec4 ambientLightColor{ 1.0f,1.0f, 1.0f,0.02f };
        glm::vec3 lightPosition{ -1.0f };
        alignas(16) glm::vec4 lightColor{ 1.0f, 1.0f, 1.0f, 5.f }; //4th component is the light intensity

    };


    App::App()
    {
        GlobalPool = VulkanDescriptorPool::Builder(Device).SetMaxSets(Swapchain::MAX_FRAMES_IN_FLIGHT)
            .AddPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, Swapchain::MAX_FRAMES_IN_FLIGHT).Build();
        LoadGameObjects();
    }

    App::~App()
    {
    }



void App::run()
{
    std::vector<std::unique_ptr<VulkanBufferObjects>> uboBuffers(Swapchain::MAX_FRAMES_IN_FLIGHT);
    for (int i = 0; i < uboBuffers.size(); i++) {
        uboBuffers[i] = std::make_unique<VulkanBufferObjects>(
            Device,
            sizeof(GlobalUBO),
            1,
            VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);
        uboBuffers[i]->Map();
    }
    
    auto GlobalSetLayout = VulkanDescriptorSetLayout::Builder(Device)
        .AddBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,  VK_SHADER_STAGE_VERTEX_BIT).Build();

    std::vector<VkDescriptorSet> GlobalDescriptorSets(Swapchain::MAX_FRAMES_IN_FLIGHT);

    for (int i = 0; i < GlobalDescriptorSets.size(); i++)
    {
        auto BufferInfo = uboBuffers[i]->DescriptorInfo();
        VulkanDescriptorWriter(*GlobalSetLayout, *GlobalPool).WriteBuffer(0, &BufferInfo).Build(GlobalDescriptorSets[i]);
    }

	ShaderSystem ShaderSys{Device, renderer.GetSwapchainRenderPass(), GlobalSetLayout->GetDescriptorSetLayout()};
    Camera camera{};
    //camera.SetViewDir(glm::vec3(0.0f, -0.5f, -2.0f), glm::vec3(0.0f, 0.0f, 2.5f));
    
    auto ViewerObject = GameObject::CreateGameObject();
    ViewerObject.Transform.Translation.z = -2.5f;
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
        camera.SetPerspectiveProj(glm::radians(50.0f), AspectR, 0.1f, 100.0f);


		if (auto CommandBuffer = renderer.BeginFrame())
		{
            int FrameIndex = renderer.GetFrameIndex();

            FrameInfo frameInfo{ FrameIndex, FrameTime, CommandBuffer, camera, GlobalDescriptorSets[FrameIndex] };

            //Update Buffers
            GlobalUBO ubo{};
            ubo.ProjectionView = camera.GetProjMat() * camera.GetViewMat();
            uboBuffers[FrameIndex]->WriteToBuffer(&ubo);
            uboBuffers[FrameIndex]->Flush();
            //Render
			renderer.BeginSwapchainRenderPass(CommandBuffer);
			ShaderSys.RenderGameObjects(frameInfo, GameObjects);
			renderer.EndSwapchainRenderPass(CommandBuffer);
			renderer.EndFrame();
		}
	}

	vkDeviceWaitIdle(Device.device());
}

}

namespace vlkn {



    void App::LoadGameObjects()
    {
        std::shared_ptr<Model> model = Model::CreateModelFromObj(Device, "./models/smooth_vase.obj");

        auto GameObj = GameObject::CreateGameObject();
        GameObj.Model = model;

        GameObj.Transform.Translation = { 0.0f, 0.5f, 0.5f };
        GameObj.Transform.Scale = { 0.5f, 0.5f, 0.5f };
        GameObjects.push_back(std::move(GameObj));

        model = Model::CreateModelFromObj(Device, "./models/quad.obj");
        auto Floor = GameObject::CreateGameObject();
        Floor.Model = model;

        Floor.Transform.Translation = { 0.0f, 0.5f, 0.0f };
        Floor.Transform.Scale = { 3.0f, 0.5f, 3.0f };
        GameObjects.push_back(std::move(Floor));
    }
}
