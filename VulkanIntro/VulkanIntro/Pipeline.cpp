#include "Pipeline.hpp"
#include "Model.hpp"

#include <fstream>
#include <stdexcept>
#include <iostream>

vlkn::Pipeline::Pipeline(VulkanDevice& Device, const std::string& VertFilePath, const std::string& FragFilePath, const PipelineConfigInfo& ConfigInfo) :
 Device{ Device }
{
	CreateGraphicsPipeline(VertFilePath, FragFilePath, ConfigInfo);
}


vlkn::Pipeline::~Pipeline()
{
	vkDestroyShaderModule(Device.device(), VertShaderModule, nullptr);
	vkDestroyShaderModule(Device.device(), FragShaderModule, nullptr);
	vkDestroyPipeline(Device.device(), GraphicsPipeline, nullptr);
}

void vlkn::Pipeline::bind(VkCommandBuffer CommandBuffer)
{
	vkCmdBindPipeline(CommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, GraphicsPipeline);
}

void vlkn::Pipeline::DefaultPipelineConfigInfo(
	PipelineConfigInfo& ConfigInfo)
{

	ConfigInfo.inputAssemblyInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	ConfigInfo.inputAssemblyInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
	ConfigInfo.inputAssemblyInfo.primitiveRestartEnable = VK_FALSE;

	ConfigInfo.viewportInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	ConfigInfo.viewportInfo.viewportCount = 1;
	ConfigInfo.viewportInfo.pViewports = nullptr;
	ConfigInfo.viewportInfo.scissorCount = 1;
	ConfigInfo.viewportInfo.pScissors = nullptr;
	

	ConfigInfo.rasterizationInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	ConfigInfo.rasterizationInfo.depthClampEnable = VK_FALSE;
	ConfigInfo.rasterizationInfo.rasterizerDiscardEnable = VK_FALSE;
	ConfigInfo.rasterizationInfo.polygonMode = VK_POLYGON_MODE_FILL;
	ConfigInfo.rasterizationInfo.lineWidth = 1.0f;
	ConfigInfo.rasterizationInfo.cullMode = VK_CULL_MODE_NONE;
	ConfigInfo.rasterizationInfo.frontFace = VK_FRONT_FACE_CLOCKWISE;
	ConfigInfo.rasterizationInfo.depthBiasEnable = VK_FALSE;
	ConfigInfo.rasterizationInfo.depthBiasConstantFactor = 0.0f;  // Optional
	ConfigInfo.rasterizationInfo.depthBiasClamp = 0.0f;           // Optional
	ConfigInfo.rasterizationInfo.depthBiasSlopeFactor = 0.0f;     // Optional

	ConfigInfo.multisampleInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	ConfigInfo.multisampleInfo.sampleShadingEnable = VK_FALSE;
	ConfigInfo.multisampleInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
	ConfigInfo.multisampleInfo.minSampleShading = 1.0f;           // Optional
	ConfigInfo.multisampleInfo.pSampleMask = nullptr;             // Optional
	ConfigInfo.multisampleInfo.alphaToCoverageEnable = VK_FALSE;  // Optional
	ConfigInfo.multisampleInfo.alphaToOneEnable = VK_FALSE;       // Optional

	ConfigInfo.colorBlendAttachment.colorWriteMask =
		VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT |
		VK_COLOR_COMPONENT_A_BIT;
	ConfigInfo.colorBlendAttachment.blendEnable = VK_FALSE;
	ConfigInfo.colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;   // Optional
	ConfigInfo.colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;  // Optional
	ConfigInfo.colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;              // Optional
	ConfigInfo.colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;   // Optional
	ConfigInfo.colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;  // Optional
	ConfigInfo.colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;              // Optional

	ConfigInfo.colorBlendInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	ConfigInfo.colorBlendInfo.logicOpEnable = VK_FALSE;
	ConfigInfo.colorBlendInfo.logicOp = VK_LOGIC_OP_COPY;  // Optional
	ConfigInfo.colorBlendInfo.attachmentCount = 1;
	ConfigInfo.colorBlendInfo.pAttachments = &ConfigInfo.colorBlendAttachment;
	ConfigInfo.colorBlendInfo.blendConstants[0] = 0.0f;  // Optional
	ConfigInfo.colorBlendInfo.blendConstants[1] = 0.0f;  // Optional
	ConfigInfo.colorBlendInfo.blendConstants[2] = 0.0f;  // Optional
	ConfigInfo.colorBlendInfo.blendConstants[3] = 0.0f;  // Optional

	ConfigInfo.depthStencilInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
	ConfigInfo.depthStencilInfo.depthTestEnable = VK_TRUE;
	ConfigInfo.depthStencilInfo.depthWriteEnable = VK_TRUE;
	ConfigInfo.depthStencilInfo.depthCompareOp = VK_COMPARE_OP_LESS;
	ConfigInfo.depthStencilInfo.depthBoundsTestEnable = VK_FALSE;
	ConfigInfo.depthStencilInfo.minDepthBounds = 0.0f;  // Optional
	ConfigInfo.depthStencilInfo.maxDepthBounds = 1.0f;  // Optional
	ConfigInfo.depthStencilInfo.stencilTestEnable = VK_FALSE;
	ConfigInfo.depthStencilInfo.front = {};  // Optional
	ConfigInfo.depthStencilInfo.back = {};   // Optional


	ConfigInfo.DynamicStateEnables = { VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR };
	ConfigInfo.DynamicStateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
	ConfigInfo.DynamicStateInfo.pDynamicStates = ConfigInfo.DynamicStateEnables.data();
	ConfigInfo.DynamicStateInfo.dynamicStateCount =
		static_cast<uint32_t>(ConfigInfo.DynamicStateEnables.size());
	ConfigInfo.DynamicStateInfo.flags = 0;

}

std::vector<char> vlkn::Pipeline::ReadFile(const std::string& FilePath)
{
	std::ifstream file{ FilePath, std::ios::ate | std::ios::binary };

	if (!file.is_open())
	{
		throw std::runtime_error("Failed to Open File: " + FilePath);
	}

	size_t FileSize = static_cast<size_t>(file.tellg());


	std::vector<char> buffer(FileSize);

	file.seekg(0);
	file.read(buffer.data(), FileSize);

	file.close();
	return buffer;
}

void vlkn::Pipeline::CreateGraphicsPipeline(const std::string& VertFilePath, const std::string& FragFilePath, const PipelineConfigInfo& ConfigInfo)
{
	auto VertCode = ReadFile(VertFilePath);
	auto FragCode = ReadFile(FragFilePath);

	CreateShaderModule(VertCode, &VertShaderModule);
	CreateShaderModule(FragCode, &FragShaderModule);

	VkPipelineShaderStageCreateInfo ShaderStages[2];
	ShaderStages[0].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	ShaderStages[0].stage = VK_SHADER_STAGE_VERTEX_BIT;
	ShaderStages[0].module = VertShaderModule;
	ShaderStages[0].pName = "main";
	ShaderStages[0].flags = 0;
	ShaderStages[0].pNext = nullptr;
	ShaderStages[0].pSpecializationInfo = nullptr;

	ShaderStages[1].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	ShaderStages[1].stage = VK_SHADER_STAGE_FRAGMENT_BIT;
	ShaderStages[1].module = FragShaderModule;
	ShaderStages[1].pName = "main";
	ShaderStages[1].flags = 0;
	ShaderStages[1].pNext = nullptr;
	ShaderStages[1].pSpecializationInfo = nullptr;

	auto BindingDescriptions = Model::Vertex::GetBindingDescriptions();
	auto AttrDescriptions = Model::Vertex::GetAtributeDescriptions();

	VkPipelineVertexInputStateCreateInfo VertexInputInfo{};
	VertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	VertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(AttrDescriptions.size());
	VertexInputInfo.vertexBindingDescriptionCount = static_cast<uint32_t>(BindingDescriptions.size());
	VertexInputInfo.pVertexAttributeDescriptions = AttrDescriptions.data();
	VertexInputInfo.pVertexBindingDescriptions = BindingDescriptions.data();

	VkGraphicsPipelineCreateInfo PipelineInfo{};
	PipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	PipelineInfo.stageCount = 2;
	PipelineInfo.pStages = ShaderStages;
	PipelineInfo.pVertexInputState = &VertexInputInfo;
	PipelineInfo.pInputAssemblyState = &ConfigInfo.inputAssemblyInfo;
	PipelineInfo.pViewportState = &ConfigInfo.viewportInfo;
	PipelineInfo.pRasterizationState = &ConfigInfo.rasterizationInfo;
	PipelineInfo.pMultisampleState = &ConfigInfo.multisampleInfo;
	PipelineInfo.pColorBlendState = &ConfigInfo.colorBlendInfo;
	PipelineInfo.pDepthStencilState = &ConfigInfo.depthStencilInfo;
	PipelineInfo.pDynamicState = &ConfigInfo.DynamicStateInfo;


	PipelineInfo.layout = ConfigInfo.pipelineLayout;
	PipelineInfo.renderPass = ConfigInfo.renderPass;
	PipelineInfo.subpass = ConfigInfo.subpass;

	PipelineInfo.basePipelineHandle = VK_NULL_HANDLE;
	PipelineInfo.basePipelineIndex = -1;


	if (vkCreateGraphicsPipelines(Device.device(), VK_NULL_HANDLE, 1, &PipelineInfo, nullptr, &GraphicsPipeline) != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to Create Graphics Pipeline");
	}

}

void vlkn::Pipeline::CreateShaderModule(const std::vector<char>& code, VkShaderModule* ShaderModule)
{
	VkShaderModuleCreateInfo CreateInfo{};
	CreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	CreateInfo.codeSize = code.size();
	CreateInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

	if (vkCreateShaderModule(Device.device(), &CreateInfo, nullptr, ShaderModule) != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to Create Shader Module");
	}


}

