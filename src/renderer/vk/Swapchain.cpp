#include "renderer/vk/Init.h"
#include "renderer/vk/Swapchain.h"
#include "renderer/vk/Renderer.h"

namespace vk {

    Swapchain::Swapchain(const VulkanContext& context, const Window& window) {

        mSwapChainSupportDetails = init::querySwapChainSupport(context.physical_device, context.surface);
        mSurfaceFormat = init::chooseSwapSurfaceFormat(mSwapChainSupportDetails.formats);
        mPresentMode = init::chooseSwapPresentMode(mSwapChainSupportDetails.presentModes);
        mExtent = init::chooseSwapExtent(mSwapChainSupportDetails.capabilities, window.mWidth, window.mHeight);

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
    }

}