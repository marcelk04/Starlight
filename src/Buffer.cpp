#include "renderer/wrapper/Buffer.hpp"

#include "Core/Asserts.hpp"

#include <cstring>

namespace stl {

Buffer::Buffer(Device& device, VkDeviceSize instanceSize, uint32_t instanceCount, VkBufferUsageFlags usageFlags, VkMemoryPropertyFlags memoryPropertyFlags, VkDeviceSize minOffsetAlignment)
	: m_Device{ device }, m_InstanceSize{ instanceSize }, m_InstanceCount{ instanceCount }, m_UsageFlags{ usageFlags }, m_MemoryPropertyFlags{ memoryPropertyFlags } {
	m_AlignmentSize = getAlignment(m_InstanceSize, minOffsetAlignment);
	m_BufferSize = m_AlignmentSize * m_InstanceCount;

	device.createBuffer(m_BufferSize, m_UsageFlags, m_MemoryPropertyFlags, m_Buffer, m_Memory);
}

Buffer::~Buffer() {
	unmap();

	vkDestroyBuffer(m_Device.getDevice(), m_Buffer, nullptr);
	vkFreeMemory(m_Device.getDevice(), m_Memory, nullptr);
}

VkResult Buffer::map(VkDeviceSize size, VkDeviceSize offset) {
	SASSERT_MSG(m_Buffer && m_Memory, "Called map on buffer before create");

	return vkMapMemory(m_Device.getDevice(), m_Memory, offset, size, 0, &m_Mapped);
}

void Buffer::unmap() {
	if (m_Mapped) {
		vkUnmapMemory(m_Device.getDevice(), m_Memory);
		m_Mapped = nullptr;
	}
}

void Buffer::writeToBuffer(void* data, VkDeviceSize size, VkDeviceSize offset) {
	SASSERT_MSG(m_Mapped, "Cannot copy to unmapped buffer");

	if (size == VK_WHOLE_SIZE) {
		memcpy(m_Mapped, data, m_BufferSize);
	}
	else {
		char* memOffset = (char*)m_Mapped;
		memOffset += offset;
		memcpy(memOffset, data, size);
	}
}

VkResult Buffer::flush(VkDeviceSize size, VkDeviceSize offset) {
	VkMappedMemoryRange mappedRange{};
	mappedRange.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
	mappedRange.memory = m_Memory;
	mappedRange.offset = offset;
	mappedRange.size = size;

	return vkFlushMappedMemoryRanges(m_Device.getDevice(), 1, &mappedRange);
}

VkDescriptorBufferInfo Buffer::descriptorInfo(VkDeviceSize size, VkDeviceSize offset) {
	return VkDescriptorBufferInfo{ m_Buffer, offset, size };
}

VkResult Buffer::invalidate(VkDeviceSize size, VkDeviceSize offset) {
	VkMappedMemoryRange mappedRange{};
	mappedRange.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
	mappedRange.memory = m_Memory;
	mappedRange.offset = offset;
	mappedRange.size = size;

	return vkInvalidateMappedMemoryRanges(m_Device.getDevice(), 1, &mappedRange);
}

void Buffer::writeToIndex(void* data, int index) {
	writeToBuffer(data, m_InstanceSize, index * m_AlignmentSize);
}

VkResult Buffer::flushIndex(int index) {
	return flush(m_AlignmentSize, index * m_AlignmentSize);
}

VkDescriptorBufferInfo Buffer::descriptorInfoForIndex(int index) {
	return descriptorInfo(m_AlignmentSize, index * m_AlignmentSize);
}

VkResult Buffer::invalidateIndex(int index) {
	return invalidate(m_AlignmentSize, index * m_AlignmentSize);
}

VkDeviceSize Buffer::getAlignment(VkDeviceSize instanceSize, VkDeviceSize minOffsetAlignment) {
	if (minOffsetAlignment > 0) {
		return (instanceSize + minOffsetAlignment - 1) & ~(minOffsetAlignment - 1);
	}

	return instanceSize;
}

}