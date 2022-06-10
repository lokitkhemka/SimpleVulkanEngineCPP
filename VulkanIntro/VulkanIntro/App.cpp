#include "App.hpp"

#include <stdexcept>
#include <array>


vlkn::App::App()
{
	LoadModels();
	CreatePipelineLayout();
	CreatePipeline();
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

void vlkn::App::sierpinski(
	std::vector<Model::Vertex>& vertices,
	int depth,
	glm::vec2 left,
	glm::vec2 right,
	glm::vec2 top) {
	if (depth <= 0) {
		vertices.push_back({ top });
		vertices.push_back({ right });
		vertices.push_back({ left });
	}
	else {
		auto leftTop = 0.5f * (left + top);
		auto rightTop = 0.5f * (right + top);
		auto leftRight = 0.5f * (left + right);
		sierpinski(vertices, depth - 1, left, leftRight, leftTop);
		sierpinski(vertices, depth - 1, leftRight, right, rightTop);
		sierpinski(vertices, depth - 1, leftTop, rightTop, top);
	}
}

void vlkn::App::LoadModels()
{
	std::vector<Model::Vertex> vertices{
		{{0.0f, -0.5f}, {1.0f, 0.0f, 0.0f}},
	  {{0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}},
	  {{-0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}}
	};

	/*std::vector<Model::Vertex> vertices{};
	sierpinski(vertices, 5, { -0.5f, 0.5f }, { 0.5f, 0.5f }, { 0.0f, -0.5f });*/

	model = std::make_unique<Model>(Device, vertices);
}

void vlkn::App::CreatePipelineLayout()
{
	VkPipelineLayoutCreateInfo PipelineLayoutInfo{};
	PipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	PipelineLayoutInfo.setLayoutCount = 0;
	PipelineLayoutInfo.pSetLayouts = nullptr;
	PipelineLayoutInfo.pushConstantRangeCount = 0;
	PipelineLayoutInfo.pPushConstantRanges = nullptr;

	if (vkCreatePipelineLayout(Device.device(), &PipelineLayoutInfo, nullptr, &PipelineLayout) != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to Create Pipeline Layout");
	}
}

void vlkn::App::CreatePipeline()
{
	PipelineConfigInfo PipelineConfig{};
	Pipeline::DefaultPipelineConfigInfo(
		PipelineConfig,
		swapchain.width(),
		swapchain.height());

	PipelineConfig.renderPass = swapchain.getRenderPass();
	PipelineConfig.pipelineLayout = PipelineLayout;

	pipeline = std::make_unique<Pipeline>(Device, "shaders/shader.vert.spv", "shaders/shader.frag.spv", PipelineConfig);

}

void vlkn::App::CreateCommandBuffers()
{
	CommandBuffers.resize(swapchain.imageCount());
	
	VkCommandBufferAllocateInfo CBAllocateInfo{};
	CBAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	CBAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	CBAllocateInfo.commandPool = Device.getCommandPool();
	CBAllocateInfo.commandBufferCount = static_cast<uint32_t>(CommandBuffers.size());

	if (vkAllocateCommandBuffers(Device.device(), &CBAllocateInfo, CommandBuffers.data()) != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to allocate Command Buffers");
	}

	for (int i = 0; i < CommandBuffers.size(); i++)
	{
		VkCommandBufferBeginInfo BeginInfo{};

		BeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

		if (vkBeginCommandBuffer(CommandBuffers[i], &BeginInfo) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to Begin Recording Command Buffer");
		}
		VkRenderPassBeginInfo RenderPassInfo{};
		RenderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		RenderPassInfo.renderPass = swapchain.getRenderPass();
		RenderPassInfo.framebuffer = swapchain.getFrameBuffer(i);

		RenderPassInfo.renderArea.offset = { 0, 0 };
		RenderPassInfo.renderArea.extent = swapchain.getSwapChainExtent();

		std::array<VkClearValue, 2> ClearValues{};
		ClearValues[0].color = { 0.1f,0.1f,0.1f,1.0f };
		ClearValues[1].depthStencil = { 1.0f, 0 };

		RenderPassInfo.clearValueCount = static_cast<uint32_t>(ClearValues.size());
		RenderPassInfo.pClearValues = ClearValues.data();


		vkCmdBeginRenderPass(CommandBuffers[i], &RenderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
		pipeline->bind(CommandBuffers[i]);
		model->Bind(CommandBuffers[i]);
		model->Draw(CommandBuffers[i]);

		vkCmdEndRenderPass(CommandBuffers[i]);

		if (vkEndCommandBuffer(CommandBuffers[i]) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to Record Command Buffers");
		}
	}

}

void vlkn::App::DrawFrame()
{
	uint32_t ImageIndex;
	auto result = swapchain.acquireNextImage(&ImageIndex);

	if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
	{
		throw std::runtime_error("Failed to acquire Swapchain Image");
	}

	result = swapchain.submitCommandBuffers(&CommandBuffers[ImageIndex], &ImageIndex);

	if (result != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to Present Swapchain Image");
	}

}
