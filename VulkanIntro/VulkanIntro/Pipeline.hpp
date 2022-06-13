#pragma once

#include "VulkanDevice.hpp"
#include <string>
#include <vector>
namespace vlkn
{
	struct PipelineConfigInfo {
		PipelineConfigInfo(const PipelineConfigInfo&) = delete;
		PipelineConfigInfo& operator=(const PipelineConfigInfo&) = delete;
		PipelineConfigInfo() = default;

		VkPipelineViewportStateCreateInfo viewportInfo;
		VkPipelineInputAssemblyStateCreateInfo inputAssemblyInfo;
		VkPipelineRasterizationStateCreateInfo rasterizationInfo;
		VkPipelineMultisampleStateCreateInfo multisampleInfo;
		VkPipelineColorBlendAttachmentState colorBlendAttachment;
		VkPipelineColorBlendStateCreateInfo colorBlendInfo;
		VkPipelineDepthStencilStateCreateInfo depthStencilInfo;
		std::vector<VkDynamicState> DynamicStateEnables;
		VkPipelineDynamicStateCreateInfo DynamicStateInfo;
		VkPipelineLayout pipelineLayout = nullptr;
		VkRenderPass renderPass = nullptr;
		uint32_t subpass = 0;
	};
	class Pipeline {
	public:
		Pipeline(VulkanDevice &Device, const std::string& VertFilePath, const std::string& FragFilePath, const PipelineConfigInfo& ConfigInfo);

		~Pipeline();

		Pipeline(const Pipeline&) = delete;
		Pipeline& operator=(const Pipeline&) = delete;

		void bind(VkCommandBuffer CommandBuffer);
		static void DefaultPipelineConfigInfo(PipelineConfigInfo& ConfigInfo);
	private:
		static std::vector<char> ReadFile(const std::string& FilePath);
		void CreateGraphicsPipeline(const std::string& VertFilePath, const std::string& FragFilePath, const PipelineConfigInfo& ConfigInfo);

		void CreateShaderModule(const std::vector<char>& code, VkShaderModule * ShaderModule);

		VulkanDevice& Device;
		VkPipeline GraphicsPipeline;
		VkShaderModule VertShaderModule;
		VkShaderModule FragShaderModule;
	};
}