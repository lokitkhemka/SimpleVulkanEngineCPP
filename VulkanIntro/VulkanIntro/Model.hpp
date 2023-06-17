#pragma once

#include "VulkanDevice.hpp"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

#include <vector>

namespace vlkn {
	class Model
	{
	public:
		struct Vertex {
			glm::vec3 position;
			glm::vec3 color;

			static std::vector<VkVertexInputBindingDescription> GetBindingDescriptions();
			static std::vector<VkVertexInputAttributeDescription> GetAtributeDescriptions();
		};

		struct ModelData {
			std::vector<Vertex> vertices{};
			std::vector<uint32_t> indices{};
		};

		Model(VulkanDevice& Device, const Model::ModelData &Data);
		~Model();

		Model(const Model&) = delete;
		Model& operator=(const Model&) = delete;

		void Bind(VkCommandBuffer CommandBuffer);
		void Draw(VkCommandBuffer CommandBuffer);
	private:
		VulkanDevice& Device;
		VkBuffer VertexBuffer;
		VkDeviceMemory VertexBufferMemory;
		uint32_t VertexCount;

		void CreateVertexBuffers(const std::vector<Vertex> &vertices);

		bool HasIndexBuffer = false;
		VkBuffer IndexBuffer;
		VkDeviceMemory IndexBufferMemory;
		uint32_t IndexCount;
		void CreateIndexBuffer(const std::vector<uint32_t>& indices);
	};

}

