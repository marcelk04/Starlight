#pragma once

#include "renderer/wrapper/Device.hpp"

#include <memory>
#include <unordered_map>
#include <vector>

namespace stl {

class DescriptorSetLayout {
public:
	class Builder {
	public:
		Builder(Device& device);

		Builder& addBinding(uint32_t binding, VkDescriptorType descriptorType, VkShaderStageFlags stageFlags, uint32_t count = 1);

		std::unique_ptr<DescriptorSetLayout> build() const;

	private:
		Device& m_Device;

		std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> m_Bindings{};
	};

public:
	DescriptorSetLayout(Device& device, std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> bindings);
	~DescriptorSetLayout();

	DescriptorSetLayout(const DescriptorSetLayout&) = delete;
	DescriptorSetLayout& operator=(const DescriptorSetLayout&) = delete;

	VkDescriptorSetLayout getDescriptorSetLayout() const { return m_DescriptorSetLayout; }

private:
	Device& m_Device;

	VkDescriptorSetLayout m_DescriptorSetLayout;
	std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> m_Bindings;

	friend class DescriptorWriter;
};

class DescriptorPool {
public:
	class Builder {
	public:
		Builder(Device& device);

		Builder& addPoolSize(VkDescriptorType descriptorType, uint32_t count);
		Builder& setPoolFlags(VkDescriptorPoolCreateFlags flags);
		Builder& setMaxSets(uint32_t count);

		std::unique_ptr<DescriptorPool> build() const;

	private:
		Device& m_Device;

		std::vector<VkDescriptorPoolSize> m_PoolSizes{};
		uint32_t m_MaxSets = 1000;
		VkDescriptorPoolCreateFlags m_PoolFlags = 0;
	};

public:
	DescriptorPool(Device& device, uint32_t maxSets, VkDescriptorPoolCreateFlags poolFlags, const std::vector<VkDescriptorPoolSize>& poolSizes);
	~DescriptorPool();

	DescriptorPool(const DescriptorPool&) = delete;
	DescriptorPool& operator=(const DescriptorPool&) = delete;

	bool allocateDescriptor(const VkDescriptorSetLayout descriptorSetLayout, VkDescriptorSet& descriptor) const;

	void freeDescriptors(std::vector<VkDescriptorSet>& descriptors) const;

	void resetPool();

private:
	Device& m_Device;

	VkDescriptorPool m_DescriptorPool;

	friend class DescriptorWriter;
};

class DescriptorWriter {
public:
	DescriptorWriter(DescriptorSetLayout& setLayout, DescriptorPool& pool);

	DescriptorWriter& writeBuffer(uint32_t binding, VkDescriptorBufferInfo* bufferInfo);
	DescriptorWriter& writeImage(uint32_t binding, VkDescriptorImageInfo* imageInfo);

	bool build(VkDescriptorSet& set);
	void overwrite(VkDescriptorSet& set);

private:
	DescriptorSetLayout& m_SetLayout;
	DescriptorPool& m_Pool;

	std::vector<VkWriteDescriptorSet> m_Writes;
};

}