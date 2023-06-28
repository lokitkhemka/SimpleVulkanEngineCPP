#pragma once

#include "VulkanDevice.hpp"

#include <memory>
#include <unordered_map>
#include <vector>
namespace vlkn {
class VulkanDescriptorSetLayout
{
public:
	class Builder {
	public:
		Builder(VulkanDevice &device):Device{device}{}

		Builder& AddBinding( uint32_t binding, VkDescriptorType descriptorType, VkShaderStageFlags stageFlags, uint32_t count = 1);
		std::unique_ptr<VulkanDescriptorSetLayout> Build() const;
	private:
		VulkanDevice& Device;
		std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> Bindings{};
	};

	VulkanDescriptorSetLayout(VulkanDevice& device, std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> bindings);
	~VulkanDescriptorSetLayout();

	VulkanDescriptorSetLayout(const VulkanDescriptorSetLayout&) = delete;
	VulkanDescriptorSetLayout& operator=(const VulkanDescriptorSetLayout&) = delete;

	VkDescriptorSetLayout GetDescriptorSetLayout() const { return DescriptorSetLayout; }

private:
	VulkanDevice& Device;
	VkDescriptorSetLayout DescriptorSetLayout;
	std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> Bindings;

	friend class VulkanDescriptorWriter;
};

class VulkanDescriptorPool {
public:
	class Builder {
	public:
		Builder(VulkanDevice& device) : Device{ device } {}
		Builder& AddPoolSize(VkDescriptorType descriptorType, uint32_t count);
		Builder& SetPoolFlags(VkDescriptorPoolCreateFlags flags);
		Builder& SetMaxSets(uint32_t count);
		std::unique_ptr<VulkanDescriptorPool> Build() const;
	private:
		VulkanDevice& Device;
		std::vector<VkDescriptorPoolSize> PoolSizes{};
		uint32_t MaxSets = 1000;
		VkDescriptorPoolCreateFlags PoolFlags = 0;
	};

	VulkanDescriptorPool(VulkanDevice& device, uint32_t maxSets, VkDescriptorPoolCreateFlags poolFlags, const std::vector<VkDescriptorPoolSize>& poolSizes);
	~VulkanDescriptorPool();
	VulkanDescriptorPool(const VulkanDescriptorPool &) = delete;
	VulkanDescriptorPool& operator=(const VulkanDescriptorPool&) = delete;

	bool AllocateDescriptorSet(
		const VkDescriptorSetLayout descriptorSetLayout, VkDescriptorSet& descriptor) const;

	void FreeDescriptorSet(std::vector<VkDescriptorSet>& descriptors) const;

	void ResetPool();

private:
	VulkanDevice& Device;
	VkDescriptorPool DescriptorPool;

	friend class VulkanDescriptorWriter;
};

class VulkanDescriptorWriter {
public:
	VulkanDescriptorWriter(VulkanDescriptorSetLayout& setLayout, VulkanDescriptorPool& pool);

	VulkanDescriptorWriter& WriteBuffer(uint32_t binding, VkDescriptorBufferInfo* bufferInfo);
	VulkanDescriptorWriter& WriteImage(uint32_t binding, VkDescriptorImageInfo* imageInfo);

	bool Build(VkDescriptorSet& set);
	void Overwrite(VkDescriptorSet& set);

private:
	VulkanDescriptorSetLayout& SetLayout;
	VulkanDescriptorPool& Pool;
	std::vector<VkWriteDescriptorSet> Writes;
};

}