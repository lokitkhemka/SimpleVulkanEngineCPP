#include "App.hpp"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

#include <stdexcept>
#include <array>
#include <cassert>

namespace vlkn {
	struct SimplePushConstantData {
		glm::mat2 Transform{1.0f};
		glm::vec2 Offset;
		alignas(16) glm::vec3 Color;
	};

}
vlkn::App::App()
{
	LoadGameObjects();
	CreatePipelineLayout();
	RecreateSwapchain();
	CreateCommandBuffers();
}

vlkn::App::~App()
{
	vkDestroyPipelineLayout(Device.device(), PipelineLayout, nullptr);
}

void vlkn::App::run()
{
	while (!window.ShouldClose())
	{
		glfwPollEvents();
		DrawFrame();
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

void vlkn::App::CreatePipelineLayout()
{
	VkPushConstantRange PushConstRange{};
	PushConstRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
	PushConstRange.offset = 0;
	PushConstRange.size = sizeof(SimplePushConstantData);


	VkPipelineLayoutCreateInfo PipelineLayoutInfo{};
	PipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	PipelineLayoutInfo.setLayoutCount = 0;
	PipelineLayoutInfo.pSetLayouts = nullptr;
	PipelineLayoutInfo.pushConstantRangeCount = 1;
	PipelineLayoutInfo.pPushConstantRanges = &PushConstRange;

	if (vkCreatePipelineLayout(Device.device(), &PipelineLayoutInfo, nullptr, &PipelineLayout) != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to Create Pipeline Layout");
	}
}

void vlkn::App::CreatePipeline()
{
	assert(swapchain != nullptr && "Cannot create pipeline before swapchain");
	assert(PipelineLayout != nullptr && "Cannot create pipeline before pipeline layout");
	PipelineConfigInfo PipelineConfig{};
	Pipeline::DefaultPipelineConfigInfo(
		PipelineConfig);

	PipelineConfig.renderPass = swapchain->getRenderPass();
	PipelineConfig.pipelineLayout = PipelineLayout;

	pipeline = std::make_unique<Pipeline>(Device, "shaders/shader.vert.spv", "shaders/shader.frag.spv", PipelineConfig);

}

void vlkn::App::CreateCommandBuffers()
{
	CommandBuffers.resize(swapchain->imageCount());
	
	VkCommandBufferAllocateInfo CBAllocateInfo{};
	CBAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	CBAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	CBAllocateInfo.commandPool = Device.getCommandPool();
	CBAllocateInfo.commandBufferCount = static_cast<uint32_t>(CommandBuffers.size());

	if (vkAllocateCommandBuffers(Device.device(), &CBAllocateInfo, CommandBuffers.data()) != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to allocate Command Buffers");
	}


}

void vlkn::App::FreeCommandBuffers()
{
	vkFreeCommandBuffers(Device.device(), Device.getCommandPool(), static_cast<uint32_t>(CommandBuffers.size()), CommandBuffers.data());
	CommandBuffers.clear();
}

void vlkn::App::RenderGameObjects(VkCommandBuffer CommandBuffer)
{
	pipeline->bind(CommandBuffer);

	for (auto& Obj : GameObjects)
	{
		Obj.Transform2D.Rotation = glm::mod(Obj.Transform2D.Rotation + 0.001f, glm::two_pi<float>());
		SimplePushConstantData Push{};
		Push.Offset = Obj.Transform2D.Translation;
		Push.Color = Obj.Color;
		Push.Transform = Obj.Transform2D.Mat2();

		vkCmdPushConstants(CommandBuffer, PipelineLayout, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 0, sizeof(SimplePushConstantData), &Push);
		Obj.Model->Bind(CommandBuffer);
		Obj.Model->Draw(CommandBuffer);
	}
}


void vlkn::App::DrawFrame()
{
	uint32_t ImageIndex;
	auto result = swapchain->acquireNextImage(&ImageIndex);

	if (result == VK_ERROR_OUT_OF_DATE_KHR)
	{
		RecreateSwapchain();
		return;
	}

	if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
	{
		throw std::runtime_error("Failed to acquire Swapchain Image");
	}

	RecordCommandBuffers(ImageIndex);
	result = swapchain->submitCommandBuffers(&CommandBuffers[ImageIndex], &ImageIndex);
	if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || window.WasWindowResized()) {
		window.ResetWindowResizedFlag();
		RecreateSwapchain();
		return;
	}
	if (result != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to Present Swapchain Image");
	}

}

void vlkn::App::RecreateSwapchain()
{
	auto extent = window.getExtent();
	while (extent.width == 0 || extent.height == 0) {
		extent = window.getExtent();
		glfwWaitEvents();
	}

	vkDeviceWaitIdle(Device.device());

	if (swapchain == nullptr)
	{
		swapchain = std::make_unique<Swapchain>(Device, extent);
	}
	else {
		swapchain = std::make_unique<Swapchain>(Device, extent, std::move(swapchain));
		if (swapchain->imageCount() != CommandBuffers.size())
		{
			FreeCommandBuffers();
			CreateCommandBuffers();
		}
	}
	
	CreatePipeline();
}

void vlkn::App::RecordCommandBuffers(int ImageIndex)
{
	VkCommandBufferBeginInfo BeginInfo{};

	BeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

	if (vkBeginCommandBuffer(CommandBuffers[ImageIndex], &BeginInfo) != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to Begin Recording Command Buffer");
	}
	VkRenderPassBeginInfo RenderPassInfo{};
	RenderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	RenderPassInfo.renderPass = swapchain->getRenderPass();
	RenderPassInfo.framebuffer = swapchain->getFrameBuffer(ImageIndex);

	RenderPassInfo.renderArea.offset = { 0, 0 };
	RenderPassInfo.renderArea.extent = swapchain->getSwapChainExtent();

	std::array<VkClearValue, 2> ClearValues{};
	ClearValues[0].color = { 0.01f,0.01f,0.01f,1.0f };
	ClearValues[1].depthStencil = { 1.0f, 0 };

	RenderPassInfo.clearValueCount = static_cast<uint32_t>(ClearValues.size());
	RenderPassInfo.pClearValues = ClearValues.data();


	vkCmdBeginRenderPass(CommandBuffers[ImageIndex], &RenderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

	VkViewport viewport{};
	viewport.x = 0.0f;
	viewport.y = 0.0f;
	viewport.width = static_cast<float>(swapchain->getSwapChainExtent().width);
	viewport.height = static_cast<float>(swapchain->getSwapChainExtent().height);
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;
	VkRect2D scissor{ {0, 0}, swapchain->getSwapChainExtent() };
	vkCmdSetViewport(CommandBuffers[ImageIndex], 0, 1, &viewport);
	vkCmdSetScissor(CommandBuffers[ImageIndex], 0, 1, &scissor);

	RenderGameObjects(CommandBuffers[ImageIndex]);

	vkCmdEndRenderPass(CommandBuffers[ImageIndex]);

	if (vkEndCommandBuffer(CommandBuffers[ImageIndex]) != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to Record Command Buffers");
	}
}

