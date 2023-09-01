#include "Init.h"
#include "Swapchain.h"
#include "VulkanRenderer.h"

namespace vk {

    Swapchain::Swapchain(const VulkanContext& context, VkRenderPass renderpass) {

        mSwapChainSupportDetails = init::querySwapChainSupport(context.physical_device, context.surface);
        mSurfaceFormat = init::chooseSwapSurfaceFormat(mSwapChainSupportDetails.formats);
        mPresentMode = init::chooseSwapPresentMode(mSwapChainSupportDetails.presentModes);

        int width, height;
        g_window->get_frame_buffer_size(&width, &height);
        mExtent = init::chooseSwapExtent(mSwapChainSupportDetails.capabilities, width, height);

         uint32_t imageCount = mSwapChainSupportDetails.capabilities.minImageCount + 1;
         if (mSwapChainSupportDetails.capabilities.maxImageCount > 0 && imageCount > mSwapChainSupportDetails.capabilities.maxImageCount) {
             imageCount = mSwapChainSupportDetails.capabilities.maxImageCount;
         }

         VkSwapchainCreateInfoKHR createInfo{};
         createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
         createInfo.surface = context.surface;
         createInfo.minImageCount = imageCount;
         createInfo.imageFormat = mSurfaceFormat.format;
         createInfo.imageColorSpace = mSurfaceFormat.colorSpace;
         createInfo.imageExtent = mExtent;
         createInfo.imageArrayLayers = 1;
         createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

         QueueFamilyIndices indices = init::findQueueFamilies(context.physical_device, context.surface);
         uint32_t queueFamilyIndices[] = { indices.graphicsFamily.value(), indices.presentFamily.value() };

         if (indices.graphicsFamily != indices.presentFamily) {
             createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
             createInfo.queueFamilyIndexCount = 2;
             createInfo.pQueueFamilyIndices = queueFamilyIndices;
         }
         else {
             createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
         }
         createInfo.preTransform = mSwapChainSupportDetails.capabilities.currentTransform;
         createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
         createInfo.presentMode = mPresentMode;
         createInfo.clipped = VK_TRUE;
         createInfo.oldSwapchain = VK_NULL_HANDLE;

         assert(vkCreateSwapchainKHR(context.device, &createInfo, nullptr, &mSwapchain) == VK_SUCCESS && "failed to create swap chain!");

		 vkGetSwapchainImagesKHR(context.device, mSwapchain, &imageCount, nullptr);
		 mSwapchainImages.resize(imageCount);
		 vkGetSwapchainImagesKHR(context.device, mSwapchain, &imageCount, mSwapchainImages.data());

         mSwapchainImageViews.resize(mSwapchainImages.size());
         for (uint32_t i = 0; i < mSwapchainImages.size(); i++) {
             mSwapchainImageViews[i] = init::createImageView(context.device, mSwapchainImages[i], mSurfaceFormat.format, VK_IMAGE_ASPECT_COLOR_BIT, 1);
         }

         m_color_image = std::make_shared<Image>(context, mExtent.width, mExtent.height, 1, context.msaa_samples,mSurfaceFormat.format, VK_IMAGE_TILING_OPTIMAL,
             VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT, VK_IMAGE_ASPECT_COLOR_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
         m_depth_image = std::make_shared<Image>(context, mExtent.width, mExtent.height, 1, context.msaa_samples, vk::init::find_depth_format(context), VK_IMAGE_TILING_OPTIMAL,
             VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, VK_IMAGE_ASPECT_DEPTH_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

         mSwapchainFramebuffers.resize(mSwapchainImageViews.size());
         for (size_t i = 0; i < mSwapchainImageViews.size(); i++) {
             std::array<VkImageView, 3> attachments = {
                 m_color_image->m_view,
                 m_depth_image->m_view,
                 mSwapchainImageViews[i]
             };

             VkFramebufferCreateInfo framebufferInfo{};
             framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
             framebufferInfo.renderPass = renderpass;
             framebufferInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
             framebufferInfo.pAttachments = attachments.data();
             framebufferInfo.width = mExtent.width;
             framebufferInfo.height = mExtent.height;
             framebufferInfo.layers = 1;

             if (vkCreateFramebuffer(context.device, &framebufferInfo, nullptr, &mSwapchainFramebuffers[i]) != VK_SUCCESS) {
                 throw std::runtime_error("failed to create framebuffer!");
             }
         }
    }
}