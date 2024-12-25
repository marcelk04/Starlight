#include "renderer/wrapper/Image.hpp"

#include "Core/Logger.hpp"
#include "Core/Asserts.hpp"

namespace stl {

Image::Image(Device& device, VkImage image, VkFormat format, VkImageViewType viewType, VkImageAspectFlagBits aspectMask)
    : m_Device{ device }, m_Image{ image }, m_Format{ format }, m_ViewType{ viewType }, m_AspectMask{ aspectMask }, m_DestroyImage{ false } {
    createView();
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

void Image::createView() {
    SASSERT_MSG(m_Image != VK_NULL_HANDLE, "Cannot create image view without image.");

    VkImageViewCreateInfo imageViewInfo = {};
    imageViewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    imageViewInfo.image = m_Image;
    imageViewInfo.viewType = m_ViewType;
    imageViewInfo.format = m_Format;
    imageViewInfo.subresourceRange.aspectMask = m_AspectMask;
    imageViewInfo.subresourceRange.baseMipLevel = 0;
    imageViewInfo.subresourceRange.levelCount = 1;
    imageViewInfo.subresourceRange.baseArrayLayer = 0;
    imageViewInfo.subresourceRange.layerCount = 1;

    SASSERT(vkCreateImageView(m_Device.getDevice(), &imageViewInfo, nullptr, &m_ImageView) == VK_SUCCESS);
}
    
}