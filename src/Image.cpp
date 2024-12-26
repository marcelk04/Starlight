#include "renderer/wrapper/Image.hpp"

#include "Core/Logger.hpp"
#include "Core/Asserts.hpp"

namespace stl {

Image::Image(Device& device, Spec& spec) 
    : m_Device{ device }, m_DestroyImage{ true } {
    createImage(spec);
    allocateMemory(spec);
    createView(spec);
}

Image::Image(Device& device, Spec& spec, VkImage image)
    : m_Device{ device }, m_Image{ image }, m_DestroyImage{ false } {
    allocateMemory(spec);
    createView(spec);
}

Image::~Image() {
    if (m_ImageMemory != VK_NULL_HANDLE) {
        vkFreeMemory(m_Device.getDevice(), m_ImageMemory, nullptr);
    }

    if (m_ImageView != VK_NULL_HANDLE) {
        vkDestroyImageView(m_Device.getDevice(), m_ImageView, nullptr);
    }

    if (m_Image != VK_NULL_HANDLE && m_DestroyImage) {
        vkDestroyImage(m_Device.getDevice(), m_Image, nullptr);
    }
}

void Image::createImage(Spec& spec) {
    VkImageCreateInfo imageInfo = {};
    imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    imageInfo.imageType = spec.imageType;
    imageInfo.extent = spec.extent;
    imageInfo.mipLevels = 1;
    imageInfo.arrayLayers = 1;
    imageInfo.format = spec.format;
    imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
    imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    imageInfo.usage = spec.usage;
    imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
    imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    imageInfo.flags = 0;

    SASSERT(vkCreateImage(m_Device.getDevice(), &imageInfo, nullptr, &m_Image) == VK_SUCCESS);
}

void Image::allocateMemory(Spec& spec) {
    if (!spec.allocateMemory) return;

    VkMemoryRequirements memRequirements;
	vkGetImageMemoryRequirements(m_Device.getDevice(), m_Image, &memRequirements);

	VkMemoryAllocateInfo allocInfo = {};
	allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocInfo.allocationSize = memRequirements.size;
	allocInfo.memoryTypeIndex = m_Device.findMemoryType(memRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

	SASSERT(vkAllocateMemory(m_Device.getDevice(), &allocInfo, nullptr, &m_ImageMemory) == VK_SUCCESS);
    SASSERT(vkBindImageMemory(m_Device.getDevice(), m_Image, m_ImageMemory, 0) == VK_SUCCESS);
}

void Image::createView(Spec& spec) {
    SASSERT_MSG(m_Image != VK_NULL_HANDLE, "Cannot create image view without image.");

    VkImageViewCreateInfo imageViewInfo = {};
    imageViewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    imageViewInfo.image = m_Image;
    imageViewInfo.viewType = spec.viewType;
    imageViewInfo.format = spec.format;
    imageViewInfo.subresourceRange.aspectMask = spec.aspectMask;
    imageViewInfo.subresourceRange.baseMipLevel = 0;
    imageViewInfo.subresourceRange.levelCount = 1;
    imageViewInfo.subresourceRange.baseArrayLayer = 0;
    imageViewInfo.subresourceRange.layerCount = 1;

    SASSERT(vkCreateImageView(m_Device.getDevice(), &imageViewInfo, nullptr, &m_ImageView) == VK_SUCCESS);
}
    
}