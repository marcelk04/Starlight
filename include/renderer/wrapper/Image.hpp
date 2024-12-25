#pragma once

#include "renderer/wrapper/Device.hpp"

#include <vulkan/vulkan.h>

namespace stl {

class Image {
public:
    Image(Device& device, VkImage image, VkFormat format, VkImageViewType viewType = VK_IMAGE_VIEW_TYPE_2D, VkImageAspectFlagBits aspectMask = VK_IMAGE_ASPECT_COLOR_BIT);
    ~Image();

    VkImage getImage() const { return m_Image; }
    VkImageView getImageView() const { return m_ImageView; }
    VkDeviceMemory getImageMemory() const { return m_ImageMemory; }

private:
    void createView();

private:
    Device& m_Device;

    VkImage m_Image{ VK_NULL_HANDLE };
    VkImageView m_ImageView{ VK_NULL_HANDLE };
    VkDeviceMemory m_ImageMemory{ VK_NULL_HANDLE };

    VkFormat m_Format;
    VkImageViewType m_ViewType;
    VkImageAspectFlagBits m_AspectMask;

    bool m_DestroyImage;
};

}