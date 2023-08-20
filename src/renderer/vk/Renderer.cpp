#include "renderer/vk/Renderer.h"
#include "renderer/vk/Init.h"
#include "renderer/vk/ValidationLayers.h"

#include <vector>

#include "GLFW/glfw3.h"

#define VALIDATION_LAYERS_ENABLE true

namespace vk {
	Renderer gRenderer;

	void Renderer::init(const Window& window) {
		init::createInstance(VALIDATION_LAYERS_ENABLE, window);
		setupDebugMessenger(mInstance, &mDebugMessenger, VALIDATION_LAYERS_ENABLE);

		window.CreateSurface(mInstance, &mSurface);
		init::createPhysicalDevice(mInstance, mMsaaSamples, mSurface);
		init::createLogicalDevice(mPhysicalDevice, mSurface, VALIDATION_LAYERS_ENABLE);

		QueueFamilyIndices indices = init::findQueueFamilies(mPhysicalDevice, mSurface);
		vkGetDeviceQueue(mDevice, indices.graphicsFamily.value(), 0, &mGraphicsQueue);
		vkGetDeviceQueue(mDevice, indices.presentFamily.value(), 0, &mPresentQueue);

		mSwapchain = std::make_unique<Swapchain>(mPhysicalDevice, mDevice, mSurface, window);
	}
	void Renderer::destroy(){
	}
}
