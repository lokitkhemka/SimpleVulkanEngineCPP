#include "Renderer.hpp"

#include <stdexcept>
#include <array>
#include <cassert>

vlkn::Renderer::Renderer(Window& window, VulkanDevice& Device)
	:
	window{window},
	Device{Device}
{
	RecreateSwapchain();
	CreateCommandBuffers();
}

vlkn::Renderer::~Renderer()
{
	FreeCommandBuffers();
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



void vlkn::Renderer::CreateCommandBuffers()
{
	CommandBuffers.resize(Swapchain::MAX_FRAMES_IN_FLIGHT);

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

void vlkn::Renderer::FreeCommandBuffers()
{
	vkFreeCommandBuffers(Device.device(), Device.getCommandPool(), static_cast<uint32_t>(CommandBuffers.size()), CommandBuffers.data());
	CommandBuffers.clear();
}


void vlkn::Renderer::RecreateSwapchain()
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
		std::shared_ptr<Swapchain> oldSwapchain = std::move(swapchain);
		swapchain = std::make_unique<Swapchain>(Device, extent, oldSwapchain);

		if (!oldSwapchain->CompareSwapFormats(*swapchain.get()))
		{
			throw std::runtime_error("Swap chain image or depth format has changed.");

		}

	}
	 
	//
}



VkCommandBuffer vlkn::Renderer::BeginFrame()
{
	assert(!IsFrameStarted && "Cannot call BeginFrame while frame is already in progress.");
	auto result = swapchain->acquireNextImage(&CurrentImgIndex);

	if (result == VK_ERROR_OUT_OF_DATE_KHR)
	{
		RecreateSwapchain();
		return nullptr;
	}

	if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
	{
		throw std::runtime_error("Failed to acquire Swapchain Image");
	}

	IsFrameStarted = true;

	auto CommandBuffer = GetCurrentCB();
	VkCommandBufferBeginInfo BeginInfo{};

	BeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

	if (vkBeginCommandBuffer(CommandBuffer, &BeginInfo) != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to Begin Recording Command Buffer");
	}
	return CommandBuffer;
}


void vlkn::Renderer::EndFrame()
{
	assert(IsFrameStarted && "Cannot call EndFrame when Frame is not in progress");
	auto CommandBuffer = GetCurrentCB();

	if (vkEndCommandBuffer(CommandBuffer) != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to Record Command Buffers");
	}

	auto result = swapchain->submitCommandBuffers(&CommandBuffer, &CurrentImgIndex);
	if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || window.WasWindowResized()) {
		window.ResetWindowResizedFlag();
		RecreateSwapchain();
	}
	else if (result != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to Present Swapchain Image");
	}
	IsFrameStarted = false;
	CurrentFrameIndex = (CurrentFrameIndex + 1) % Swapchain::MAX_FRAMES_IN_FLIGHT;
}


void vlkn::Renderer::BeginSwapchainRenderPass(VkCommandBuffer CommandBuffer)
{
	assert(IsFrameStarted && "Cannot call BeginSwapchainRenderPass if the frame is not in progress");
	assert(CommandBuffer == GetCurrentCB() && "Cannot begin RenderPass on Command Buffer from different Frame.");

	VkRenderPassBeginInfo RenderPassInfo{};
	RenderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	RenderPassInfo.renderPass = swapchain->getRenderPass();
	RenderPassInfo.framebuffer = swapchain->getFrameBuffer(CurrentImgIndex);

	RenderPassInfo.renderArea.offset = { 0, 0 };
	RenderPassInfo.renderArea.extent = swapchain->getSwapChainExtent();

	std::array<VkClearValue, 2> ClearValues{};
	ClearValues[0].color = { 0.01f,0.01f,0.01f,1.0f };
	ClearValues[1].depthStencil = { 1.0f, 0 };

	RenderPassInfo.clearValueCount = static_cast<uint32_t>(ClearValues.size());
	RenderPassInfo.pClearValues = ClearValues.data();


	vkCmdBeginRenderPass(CommandBuffer, &RenderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

	VkViewport viewport{};
	viewport.x = 0.0f;
	viewport.y = 0.0f;
	viewport.width = static_cast<float>(swapchain->getSwapChainExtent().width);
	viewport.height = static_cast<float>(swapchain->getSwapChainExtent().height);
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;
	VkRect2D scissor{ {0, 0}, swapchain->getSwapChainExtent() };
	vkCmdSetViewport(CommandBuffer, 0, 1, &viewport);
	vkCmdSetScissor(CommandBuffer, 0, 1, &scissor);

}

void vlkn::Renderer::EndSwapchainRenderPass(VkCommandBuffer CommandBuffer)
{
	assert(IsFrameStarted && "Cannot call EndSwapchainRenderPass if the frame is in progress");
	assert(CommandBuffer == GetCurrentCB() && "Cannot end RenderPass on Command Buffer from different Frame.");

	vkCmdEndRenderPass(CommandBuffer);
}

