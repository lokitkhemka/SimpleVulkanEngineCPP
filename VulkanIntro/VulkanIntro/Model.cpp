#include "Model.hpp"

#include "Utils.hpp"

#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/hash.hpp>

#include <cassert>
#include <cstring>
#include <iostream>
#include <unordered_map>


namespace std {
	template <>
	struct hash<vlkn::Model::Vertex>
	{
		size_t operator()(vlkn::Model::Vertex const& vertex) const
		{
			size_t seed = 0;
			vlkn::hashCombine(seed, vertex.position, vertex.color, vertex.normal, vertex.uv);
			return seed;
		}
	};
}

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

std::unique_ptr<vlkn::Model> vlkn::Model::CreateModelFromObj(VulkanDevice& device, const std::string& filepath)
{
	ModelData data{};
	data.LoadModel(filepath);
	std::cout << "Vertices Size: " << data.vertices.size() << "\n";
	return std::make_unique<Model>(device, data);
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
	std::vector<VkVertexInputAttributeDescription> AttrDescriptions{};

	AttrDescriptions.push_back({ 0,0,VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, position) });
	AttrDescriptions.push_back({ 1,0,VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, color) });
	AttrDescriptions.push_back({ 2,0,VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, normal) });
	AttrDescriptions.push_back({ 3,0,VK_FORMAT_R32G32_SFLOAT, offsetof(Vertex, uv) });
	
	
	return AttrDescriptions;
}

void vlkn::Model::ModelData::LoadModel(const std::string& filepath)
{
	tinyobj::attrib_t  attrib;
	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;
	std::string warn, error;

	if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &error, filepath.c_str())) {
		throw std::runtime_error(warn + error);
	}

	vertices.clear(); indices.clear();

	std::unordered_map<Vertex, uint32_t> UniqueVertices{};

	for (const auto& shape : shapes)
	{
		for (const auto& index : shape.mesh.indices)
		{
			Vertex vertex{};
			if (index.vertex_index >= 0)
			{
				vertex.position = { attrib.vertices[3 * index.vertex_index + 0],
									attrib.vertices[3 * index.vertex_index + 1],
									attrib.vertices[3 * index.vertex_index + 2] };

				vertex.color = {attrib.colors[3* index.vertex_index + 0],
								attrib.colors[3* index.vertex_index + 1], 
								attrib.colors[3* index.vertex_index + 2], };
				
			}

			if (index.normal_index >= 0)
			{
				vertex.normal = { attrib.normals[3 * index.normal_index + 0],
									attrib.normals[3 * index.normal_index + 1],
									attrib.normals[3 * index.normal_index + 2] };
			}

			if (index.texcoord_index >= 0)
			{
				vertex.uv = { attrib.texcoords[2 * index.texcoord_index + 0],
									attrib.texcoords[2 * index.texcoord_index + 1]
									};
			}

			if (UniqueVertices.count(vertex) == 0) {
				UniqueVertices[vertex] = static_cast<uint32_t>(vertices.size());
				vertices.push_back(vertex);
			}
			indices.push_back(UniqueVertices[vertex]);
		}
	}

}
