#include "pch.h"
#include "VulkanInit.h"
#include "VulkanSwapchain.h"
#include "VulkanRenderer.h"

namespace vk {

    VulkanSwapchain::VulkanSwapchain(const VulkanContext& context, VkRenderPass renderpass) {
        init::CreateSyncObjects(context, m_image_available_semaphores, m_render_finished_semaphores, m_in_flight_fences);

        m_swapchain_details = init::QuerySwapChainSupport(context.physical_device, context.surface);
        m_surface_format = init::ChooseSwapSurfaceFormat(m_swapchain_details.formats);
        m_present_mode = init::ChooseSwapPresentMode(m_swapchain_details.present_modes);

        int width, height;
        g_window->GetFrameBufferSize(&width, &height);
        m_extent = init::ChooseSwapExtent(m_swapchain_details.capabilities, width, height);

         uint32_t imageCount = m_swapchain_details.capabilities.minImageCount + 1;
         if (m_swapchain_details.capabilities.maxImageCount > 0 && imageCount > m_swapchain_details.capabilities.maxImageCount) {
             imageCount = m_swapchain_details.capabilities.maxImageCount;
         }

         VkSwapchainCreateInfoKHR createInfo{};
         createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
         createInfo.surface = context.surface;
         createInfo.minImageCount = imageCount;
         createInfo.imageFormat = m_surface_format.format;
         createInfo.imageColorSpace = m_surface_format.colorSpace;
         createInfo.imageExtent = m_extent;
         createInfo.imageArrayLayers = 1;
         createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

         QueueFamilyIndices indices = init::FindQueueFamilies(context.physical_device, context.surface);
         uint32_t queueFamilyIndices[] = { indices.graphics_family.value(), indices.present_family.value() };

         if (indices.graphics_family != indices.present_family) {
             createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
             createInfo.queueFamilyIndexCount = 2;
             createInfo.pQueueFamilyIndices = queueFamilyIndices;
         }
         else {
             createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
         }
         createInfo.preTransform = m_swapchain_details.capabilities.currentTransform;
         createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
         createInfo.presentMode = m_present_mode;
         createInfo.clipped = VK_TRUE;
         createInfo.oldSwapchain = VK_NULL_HANDLE;

         VkResult result = vkCreateSwapchainKHR(context.device, &createInfo, nullptr, &m_swapchain);
         assert(result == VK_SUCCESS && "failed to create swap chain!");

		 vkGetSwapchainImagesKHR(context.device, m_swapchain, &imageCount, nullptr);
		 m_images.resize(imageCount);
		 vkGetSwapchainImagesKHR(context.device, m_swapchain, &imageCount, m_images.data());

         m_image_views.resize(m_images.size());
         for (uint32_t i = 0; i < m_images.size(); i++) {
             m_image_views[i] = init::CreateImageView(context.device, m_images[i], m_surface_format.format, VK_IMAGE_ASPECT_COLOR_BIT, 1);
         }

         m_color_image = std::make_shared<VulkanImage>(context, m_extent.width, m_extent.height, 1, context.msaa_samples,m_surface_format.format, VK_IMAGE_TILING_OPTIMAL,
             VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT, VK_IMAGE_ASPECT_COLOR_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
         m_depth_image = std::make_shared<VulkanImage>(context, m_extent.width, m_extent.height, 1, context.msaa_samples, vk::init::FindDepthFormat(context), VK_IMAGE_TILING_OPTIMAL,
             VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, VK_IMAGE_ASPECT_DEPTH_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

         m_framebuffers.resize(m_image_views.size());
         for (size_t i = 0; i < m_image_views.size(); i++) {
             std::array<VkImageView, 3> attachments = {
                 m_color_image->m_view,
                 m_depth_image->m_view,
                 m_image_views[i]
             };

             VkFramebufferCreateInfo framebufferInfo{};
             framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
             framebufferInfo.renderPass = renderpass;
             framebufferInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
             framebufferInfo.pAttachments = attachments.data();
             framebufferInfo.width = m_extent.width;
             framebufferInfo.height = m_extent.height;
             framebufferInfo.layers = 1;

             if (vkCreateFramebuffer(context.device, &framebufferInfo, nullptr, &m_framebuffers[i]) != VK_SUCCESS) {
                 throw std::runtime_error("failed to create framebuffer!");
             }
         }
    }
    void VulkanSwapchain::GetNextImage(uint32_t current_frame) {
        vkWaitForFences(g_renderer->GetContext().device, 1, &m_in_flight_fences[current_frame], VK_TRUE, UINT64_MAX);

        VkResult result = vkAcquireNextImageKHR(g_renderer->GetContext().device, m_swapchain, UINT64_MAX, m_image_available_semaphores[current_frame], VK_NULL_HANDLE, &m_image_index);

        /* if (result == VK_ERROR_OUT_OF_DATE_KHR) {
             recreateSwapChain();
             return;
         }
         else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
             throw std::runtime_error("failed to acquire swap chain image!");
         }*/

        vkResetFences(g_renderer->GetContext().device, 1, &m_in_flight_fences[current_frame]);
    }

    void VulkanSwapchain::Submit(const std::vector<VkCommandBuffer>& submitCommandBuffers, uint32_t current_frame) {
        // Synchronization and submission of command buffer to graphics queue
        VkSubmitInfo submitInfo{};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

        VkSemaphore waitSemaphores[] = { m_image_available_semaphores[current_frame] };
        VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
        submitInfo.waitSemaphoreCount = 1;
        submitInfo.pWaitSemaphores = waitSemaphores;
        submitInfo.pWaitDstStageMask = waitStages;
        submitInfo.commandBufferCount = static_cast<uint32_t>(submitCommandBuffers.size());
        submitInfo.pCommandBuffers = submitCommandBuffers.data();

        VkSemaphore signalSemaphores[] = { m_render_finished_semaphores[current_frame] };
        submitInfo.signalSemaphoreCount = 1;
        submitInfo.pSignalSemaphores = signalSemaphores;

        if (vkQueueSubmit(g_renderer->GetContext().graphics_queue, 1, &submitInfo, m_in_flight_fences[current_frame]) != VK_SUCCESS) {
            throw std::runtime_error("failed to submit draw command buffer!");
        }

        // Signal for an image to be presented from the present queue
        VkPresentInfoKHR presentInfo{};
        presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
        presentInfo.waitSemaphoreCount = 1;
        presentInfo.pWaitSemaphores = signalSemaphores;
        VkSwapchainKHR swapChains[] = { m_swapchain };
        presentInfo.swapchainCount = 1;
        presentInfo.pSwapchains = swapChains;
        presentInfo.pImageIndices = &m_image_index;

        vkQueuePresentKHR(g_renderer->GetContext().present_queue, &presentInfo);

        //if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || framebufferResized) {
        //    framebufferResized = false;
        //    recreateSwapChain();
        //}
        //else if (result != VK_SUCCESS) {
        //    throw std::runtime_error("failed to present swap chain image!");
        //}
    }
}