#include "Model.hpp"

#include <cassert>
#include <cstring>

vlkn::Model::Model(VulkanDevice& Device, const Model::ModelData& Data): Device{Device}
{
	CreateVertexBuffers(Data.vertices);
	CreateIndexBuffer(Data.indices);
}

vlkn::Model::~Model()
{
	vkDestroyBuffer(Device.device(), VertexBuffer, nullptr);
	vkFreeMemory(Device.device(), VertexBufferMemory, nullptr);

	if (HasIndexBuffer) {
		vkDestroyBuffer(Device.device(), IndexBuffer, nullptr);
		vkFreeMemory(Device.device(), IndexBufferMemory, nullptr);
	}
}

void vlkn::Model::Bind(VkCommandBuffer CommandBuffer)
{
	VkBuffer buffers[] = { VertexBuffer };
	VkDeviceSize offsets[] = { 0 };
	vkCmdBindVertexBuffers(CommandBuffer, 0, 1, buffers, offsets);

	if (HasIndexBuffer)
	{
		vkCmdBindIndexBuffer(CommandBuffer, IndexBuffer, 0, VK_INDEX_TYPE_UINT32);
	}
}

void vlkn::Model::Draw(VkCommandBuffer CommandBuffer)
{
	if (HasIndexBuffer) {
		vkCmdDrawIndexed(CommandBuffer, IndexCount, 1, 0, 0, 0);
	}
	else {
		vkCmdDraw(CommandBuffer, VertexCount, 1, 0, 0);
	}
}

void vlkn::Model::CreateVertexBuffers(const std::vector<Vertex>& vertices)
{
	VertexCount = static_cast<uint32_t>(vertices.size());
	assert(VertexCount >= 3 && "Vertex Count must be atleast 3");
	VkDeviceSize BufferSize = sizeof(vertices[0]) * VertexCount;

	VkBuffer StagingBuffer;
	VkDeviceMemory StagingBufferMemory;

	Device.createBuffer(BufferSize,
		VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
		StagingBuffer, StagingBufferMemory);

	void* data;
	vkMapMemory(Device.device(), StagingBufferMemory, 0, BufferSize, 0, &data);
	memcpy(data, vertices.data(), static_cast<size_t>(BufferSize));
	vkUnmapMemory(Device.device(), StagingBufferMemory);

	Device.createBuffer(BufferSize,
		VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
		VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, VertexBuffer, VertexBufferMemory
		);

	Device.copyBuffer(StagingBuffer, VertexBuffer, BufferSize);

	vkDestroyBuffer(Device.device(), StagingBuffer, nullptr);
	vkFreeMemory(Device.device(), StagingBufferMemory, nullptr);
}

void vlkn::Model::CreateIndexBuffer(const std::vector<uint32_t>& indices)
{
	IndexCount = static_cast<uint32_t>(indices.size());
	HasIndexBuffer = IndexCount > 0;
	if (!HasIndexBuffer)
	{
		return;
	}
	VkDeviceSize BufferSize = sizeof(indices[0]) * IndexCount;

	VkBuffer StagingBuffer;
	VkDeviceMemory StagingBufferMemory;

	Device.createBuffer(BufferSize,
		VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
		StagingBuffer, StagingBufferMemory);

	void* data;
	vkMapMemory(Device.device(), StagingBufferMemory, 0, BufferSize, 0, &data);
	memcpy(data, indices.data(), static_cast<size_t>(BufferSize));
	vkUnmapMemory(Device.device(), StagingBufferMemory);

	Device.createBuffer(BufferSize,
		VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
		VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, IndexBuffer, IndexBufferMemory
	);

	Device.copyBuffer(StagingBuffer, IndexBuffer, BufferSize);

	vkDestroyBuffer(Device.device(), StagingBuffer, nullptr);
	vkFreeMemory(Device.device(), StagingBufferMemory, nullptr);
}


std::vector<VkVertexInputBindingDescription> vlkn::Model::Vertex::GetBindingDescriptions()
{
	std::vector<VkVertexInputBindingDescription> BindingDescriptions(1);
	BindingDescriptions[0].binding = 0;
	BindingDescriptions[0].stride = sizeof(Vertex);
	BindingDescriptions[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
	return BindingDescriptions;
}

std::vector<VkVertexInputAttributeDescription> vlkn::Model::Vertex::GetAtributeDescriptions()
{
	std::vector<VkVertexInputAttributeDescription> AttrDescriptions(2);
	AttrDescriptions[0].binding = 0;
	AttrDescriptions[0].location = 0;
	AttrDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
	AttrDescriptions[0].offset = offsetof(Vertex, position);

	AttrDescriptions[1].binding = 0;
	AttrDescriptions[1].location = 1;
	AttrDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
	AttrDescriptions[1].offset = offsetof(Vertex, color);
	return AttrDescriptions;
}
