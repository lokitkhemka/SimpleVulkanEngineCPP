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
		glm::mat2 Transform{ 1.0f };
		glm::vec2 Offset;
		alignas(16) glm::vec3 Color;
	};

}
vlkn::ShaderSystem::ShaderSystem(VulkanDevice& Device, VkRenderPass RenderPass)
	:
	Device{Device}
{
	CreatePipelineLayout();
	CreatePipeline(RenderPass);
}

vlkn::ShaderSystem::~ShaderSystem()
{
	vkDestroyPipelineLayout(Device.device(), PipelineLayout, nullptr);
}

void vlkn::ShaderSystem::CreatePipelineLayout()
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


void vlkn::ShaderSystem::RenderGameObjects(VkCommandBuffer CommandBuffer, std::vector<GameObject>& GameObjects)
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
