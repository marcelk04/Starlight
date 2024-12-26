#pragma once

#include "renderer/wrapper/Device.hpp"

#include <vulkan/vulkan.h>

namespace stl {

class Image {
public:
    struct Spec {
        VkImageType imageType = VK_IMAGE_TYPE_2D;
        VkImageViewType viewType = VK_IMAGE_VIEW_TYPE_2D;
        VkFormat format = VK_FORMAT_UNDEFINED;
        VkExtent3D extent = { 0, 0, 1 };
        VkImageUsageFlags usage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
        VkImageAspectFlags aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        bool allocateMemory = true;
    };

public:
    Image(Device& device, Spec& spec);
    Image(Device& device, Spec& spec, VkImage image);
    ~Image();

    VkImage getImage() const { return m_Image; }
    VkImageView getImageView() const { return m_ImageView; }
    VkDeviceMemory getImageMemory() const { return m_ImageMemory; }

private:
    void createImage(Spec& spec);
    void allocateMemory(Spec& spec);
    void createView(Spec& spec);

private:
    Device& m_Device;

    VkImage m_Image{ VK_NULL_HANDLE };
    VkImageView m_ImageView{ VK_NULL_HANDLE };
    VkDeviceMemory m_ImageMemory{ VK_NULL_HANDLE };

    bool m_DestroyImage;
};

}