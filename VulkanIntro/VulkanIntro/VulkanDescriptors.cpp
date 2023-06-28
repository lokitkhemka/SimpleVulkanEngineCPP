#include "VulkanDescriptors.hpp"


#include <cassert>
#include <stdexcept>

namespace vlkn {
	VulkanDescriptorSetLayout::Builder& VulkanDescriptorSetLayout::Builder::AddBinding(uint32_t binding, 
		VkDescriptorType descriptorType, 
		VkShaderStageFlags stageFlags, 
		uint32_t count)
	{
		assert(Bindings.count(binding) == 0 && "Binding Already in Use.");
		VkDescriptorSetLayoutBinding LayoutBinding{};
		LayoutBinding.binding = binding;
		LayoutBinding.descriptorType = descriptorType;
		LayoutBinding.descriptorCount = count;
		LayoutBinding.stageFlags = stageFlags;
		Bindings[binding] = LayoutBinding;
		return *this;
	}

	std::unique_ptr<VulkanDescriptorSetLayout> VulkanDescriptorSetLayout::Builder::Build() const {
		return std::make_unique<VulkanDescriptorSetLayout>(Device, Bindings);
	}


	VulkanDescriptorSetLayout::VulkanDescriptorSetLayout(VulkanDevice& device, std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> bindings):
		Device{device}, Bindings{bindings}
	{
		std::vector<VkDescriptorSetLayoutBinding> SetLayoutBindings{};
		for (auto kv : bindings) {
			SetLayoutBindings.push_back(kv.second);
		}

		VkDescriptorSetLayoutCreateInfo DescriptorSetLayoutInfo{};
		DescriptorSetLayoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		DescriptorSetLayoutInfo.bindingCount = static_cast<uint32_t>(SetLayoutBindings.size());
		DescriptorSetLayoutInfo.pBindings = SetLayoutBindings.data();

		if (vkCreateDescriptorSetLayout(Device.device(), &DescriptorSetLayoutInfo, nullptr, &DescriptorSetLayout) != VK_SUCCESS) {
			throw std::runtime_error("Failed to Create Descriptor Set Layout");
		}
	}

	VulkanDescriptorSetLayout::~VulkanDescriptorSetLayout()
	{
		vkDestroyDescriptorSetLayout(Device.device(), DescriptorSetLayout, nullptr);
	}


	VulkanDescriptorPool::Builder& VulkanDescriptorPool::Builder::AddPoolSize(VkDescriptorType descriptorType, uint32_t count)
	{
		PoolSizes.push_back({ descriptorType, count });
		return *this;
	}

	VulkanDescriptorPool::Builder& VulkanDescriptorPool::Builder::SetPoolFlags(VkDescriptorPoolCreateFlags flags)
	{
		PoolFlags = flags;
		return *this;
	}

	VulkanDescriptorPool::Builder& VulkanDescriptorPool::Builder::SetMaxSets(uint32_t count)
	{
		MaxSets = count;
		return *this;
	}

	std::unique_ptr<VulkanDescriptorPool> VulkanDescriptorPool::Builder::Build() const {
		return std::make_unique<VulkanDescriptorPool>(Device, MaxSets, PoolFlags, PoolSizes);
	}

	VulkanDescriptorPool::VulkanDescriptorPool(VulkanDevice& device, uint32_t maxSets, VkDescriptorPoolCreateFlags poolFlags, const std::vector<VkDescriptorPoolSize>& poolSizes):Device{device}
	{
		VkDescriptorPoolCreateInfo DescriptorPoolInfo{};
		DescriptorPoolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		DescriptorPoolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
		DescriptorPoolInfo.pPoolSizes = poolSizes.data();
		DescriptorPoolInfo.maxSets = maxSets;
		DescriptorPoolInfo.flags = poolFlags;

		if (vkCreateDescriptorPool(Device.device(), &DescriptorPoolInfo, nullptr, &DescriptorPool) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to Create Descriptor Pool.");
		}
	}

	VulkanDescriptorPool::~VulkanDescriptorPool()
	{
		vkDestroyDescriptorPool(Device.device(), DescriptorPool, nullptr);
	}

	bool VulkanDescriptorPool::AllocateDescriptorSet(const VkDescriptorSetLayout descriptorSetLayout, VkDescriptorSet& descriptor) const
	{
		VkDescriptorSetAllocateInfo AllocInfo{};

		AllocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
		AllocInfo.descriptorPool = DescriptorPool;
		AllocInfo.pSetLayouts = &descriptorSetLayout;
		AllocInfo.descriptorSetCount = 1;

		if (vkAllocateDescriptorSets(Device.device(), &AllocInfo, &descriptor) != VK_SUCCESS) {
			return false;
		}
		return true;
	}

	void VulkanDescriptorPool::FreeDescriptorSet(std::vector<VkDescriptorSet>& descriptors) const
	{
		vkFreeDescriptorSets(
			Device.device(),
			DescriptorPool,
			static_cast<uint32_t>(descriptors.size()),
			descriptors.data());
	}

	void VulkanDescriptorPool::ResetPool()
	{
		vkResetDescriptorPool(Device.device(), DescriptorPool, 0);
	}

	VulkanDescriptorWriter::VulkanDescriptorWriter(VulkanDescriptorSetLayout& setLayout, VulkanDescriptorPool& pool):
		SetLayout{setLayout}, Pool{pool}
	{
	}

	VulkanDescriptorWriter& VulkanDescriptorWriter::WriteBuffer(uint32_t binding, VkDescriptorBufferInfo* bufferInfo)
	{
		assert(SetLayout.Bindings.count(binding) == 1 && "Layout Doesn't contain specified Binding.");

		auto& BindingDescription = SetLayout.Bindings[binding];

		assert(BindingDescription.descriptorCount == 1 && "Binding Single Descriptor Info. However, Binding expects multiple.");
		VkWriteDescriptorSet Write{};
		Write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		Write.descriptorType = BindingDescription.descriptorType;
		Write.dstBinding = binding;
		Write.pBufferInfo = bufferInfo;
		Write.descriptorCount = 1;

		Writes.push_back(Write);
		return *this;
	}

	VulkanDescriptorWriter& VulkanDescriptorWriter::WriteImage(uint32_t binding, VkDescriptorImageInfo* imageInfo)
	{
		assert(SetLayout.Bindings.count(binding) == 1 && "Layout does not contain specified binding");

		auto& BindingDescription = SetLayout.Bindings[binding];

		assert(
			BindingDescription.descriptorCount == 1 &&
			"Binding single descriptor info, but binding expects multiple");

		VkWriteDescriptorSet Write{};
		Write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		Write.descriptorType = BindingDescription.descriptorType;
		Write.dstBinding = binding;
		Write.pImageInfo = imageInfo;
		Write.descriptorCount = 1;

		Writes.push_back(Write);
		return *this;
	}

	bool VulkanDescriptorWriter::Build(VkDescriptorSet& set)
	{
		bool success = Pool.AllocateDescriptorSet(SetLayout.GetDescriptorSetLayout(), set);
		if (!success) {
			return false;
		}
		Overwrite(set);
		return true;
	}

	void VulkanDescriptorWriter::Overwrite(VkDescriptorSet& set)
	{
		for (auto& write : Writes) {
			write.dstSet = set;
		}
		vkUpdateDescriptorSets(Pool.Device.device(), Writes.size(), Writes.data(), 0, nullptr);
	}

}
