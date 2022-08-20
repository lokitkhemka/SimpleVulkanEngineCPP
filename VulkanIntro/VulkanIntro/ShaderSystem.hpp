#pragma once

#include "Pipeline.hpp"
#include "VulkanDevice.hpp"
#include "GameObject.hpp"
#include "Camera.hpp"

#include <memory>
#include <vector>

namespace vlkn {
	class ShaderSystem{
	public:

		ShaderSystem(VulkanDevice& Device, VkRenderPass RenderPass);
		~ShaderSystem();

		ShaderSystem(const ShaderSystem&) = delete;
		ShaderSystem& operator=(const ShaderSystem&) = delete;

		void RenderGameObjects(VkCommandBuffer CommandBuffer, std::vector<GameObject>& GameObjects, const Camera& camera);
	private:
		void CreatePipelineLayout();
		void CreatePipeline(VkRenderPass RenderPass);
		
		VulkanDevice& Device;
		std::unique_ptr<Pipeline> pipeline;
		VkPipelineLayout PipelineLayout;
	};
}