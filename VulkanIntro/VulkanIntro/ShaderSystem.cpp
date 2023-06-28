#include "ShaderSystem.hpp"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

#include <stdexcept>
#include <array>
#include <cassert>

namespace vlkn {
	struct SimplePushConstantData {
		glm::mat4 ModelMatrix{ 1.0f };
		glm::mat4 NormalMatrix{ 1.0f };
	};

}
vlkn::ShaderSystem::ShaderSystem(VulkanDevice& Device, VkRenderPass RenderPass, VkDescriptorSetLayout globalSetLayout)
	:
	Device{Device}
{
	CreatePipelineLayout(globalSetLayout);
	CreatePipeline(RenderPass);
}

vlkn::ShaderSystem::~ShaderSystem()
{
	vkDestroyPipelineLayout(Device.device(), PipelineLayout, nullptr);
}

void vlkn::ShaderSystem::CreatePipelineLayout(VkDescriptorSetLayout globalSetLayout)
{
	VkPushConstantRange PushConstRange{};
	PushConstRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
	PushConstRange.offset = 0;
	PushConstRange.size = sizeof(SimplePushConstantData);

	std::vector<VkDescriptorSetLayout> DescriptorSetLayouts{ globalSetLayout };

	VkPipelineLayoutCreateInfo PipelineLayoutInfo{};
	PipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	PipelineLayoutInfo.setLayoutCount = static_cast<uint32_t>(DescriptorSetLayouts.size());
	PipelineLayoutInfo.pSetLayouts = DescriptorSetLayouts.data();
	PipelineLayoutInfo.pushConstantRangeCount = 1;
	PipelineLayoutInfo.pPushConstantRanges = &PushConstRange;

	if (vkCreatePipelineLayout(Device.device(), &PipelineLayoutInfo, nullptr, &PipelineLayout) != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to Create Pipeline Layout");
	}
}

void vlkn::ShaderSystem::CreatePipeline(VkRenderPass RenderPass)
{
	assert(PipelineLayout != nullptr && "Cannot create pipeline before pipeline layout");
	PipelineConfigInfo PipelineConfig{};
	Pipeline::DefaultPipelineConfigInfo(
		PipelineConfig);

	PipelineConfig.renderPass = RenderPass;
	PipelineConfig.pipelineLayout = PipelineLayout;

	pipeline = std::make_unique<Pipeline>(Device, "shaders/shader.vert.spv", "shaders/shader.frag.spv", PipelineConfig);

}


void vlkn::ShaderSystem::RenderGameObjects(FrameInfo & frameInfo, std::vector<GameObject>& GameObjects)
{
	pipeline->bind(frameInfo.CommandBuffer);\

		vkCmdBindDescriptorSets(
			frameInfo.CommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, PipelineLayout,
			0, 1, &frameInfo.GlobalDescriptorSet, 0, nullptr);

	for (auto& Obj : GameObjects)
	{
		SimplePushConstantData Push{};

		

		Push.ModelMatrix = Obj.Transform.Mat4();
		Push.NormalMatrix = Obj.Transform.NormalMatrix();

		vkCmdPushConstants(frameInfo.CommandBuffer, PipelineLayout, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 0, sizeof(SimplePushConstantData), &Push);
		Obj.Model->Bind(frameInfo.CommandBuffer);
		Obj.Model->Draw(frameInfo.CommandBuffer);
	}
}

