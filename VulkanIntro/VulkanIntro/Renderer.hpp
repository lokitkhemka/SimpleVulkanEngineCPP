#pragma once

#include "Window.hpp"
#include "VulkanDevice.hpp"
#include "Swapchain.hpp"


#include <memory>
#include <vector>
#include <cassert>

namespace vlkn {
	class Renderer {
	public:

		Renderer(Window &window, VulkanDevice &Device);
		~Renderer();

		Renderer(const Renderer&) = delete;
		Renderer& operator=(const Renderer&) = delete;

		VkRenderPass GetSwapchainRenderPass() const { return swapchain->getRenderPass(); }
		float GetAspectRatio() const { return swapchain->extentAspectRatio(); }
		bool IsFrameInProgress() const { return IsFrameStarted; }
		VkCommandBuffer GetCurrentCB() const 
		{
			assert(IsFrameStarted && "Cannot get Command Buffer when Frame is not in progress");
			return CommandBuffers[CurrentFrameIndex];
		}

		int GetFrameIndex() const {
			assert(IsFrameStarted && "Cannot get frame index when frame is not in progress");
			return CurrentFrameIndex;
		}
		
		VkCommandBuffer BeginFrame();
		void EndFrame();
		void BeginSwapchainRenderPass(VkCommandBuffer CommandBuffer);
		void EndSwapchainRenderPass(VkCommandBuffer CommandBuffer);


	private:
		void CreateCommandBuffers();
		void FreeCommandBuffers();
		void RecreateSwapchain();

		Window& window;
		VulkanDevice& Device;
		std::unique_ptr<Swapchain> swapchain;
		std::vector<VkCommandBuffer> CommandBuffers;
		
		uint32_t CurrentImgIndex;
		int CurrentFrameIndex{0};
		bool IsFrameStarted{false};
		
	};
}