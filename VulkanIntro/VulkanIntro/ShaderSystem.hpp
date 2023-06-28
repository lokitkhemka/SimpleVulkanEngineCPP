#pragma once

#include "Pipeline.hpp"
#include "VulkanDevice.hpp"
#include "GameObject.hpp"
#include "Camera.hpp"
#include "FrameInfo.hpp"

#include <memory>
#include <vector>

namespace vlkn {
	class ShaderSystem{
	public:

		ShaderSystem(VulkanDevice& Device, VkRenderPass RenderPass, VkDescriptorSetLayout globalSetLayout);
		~ShaderSystem();

		ShaderSystem(const ShaderSystem&) = delete;
		ShaderSystem& operator=(const ShaderSystem&) = delete;

		void RenderGameObjects(FrameInfo& frameInfo, std::vector<GameObject>& GameObjects);
	private:
		void CreatePipelineLayout(VkDescriptorSetLayout globalSetLayout);
		void CreatePipeline(VkRenderPass RenderPass);
		
		VulkanDevice& Device;
		std::unique_ptr<Pipeline> pipeline;
		VkPipelineLayout PipelineLayout;
	};
}