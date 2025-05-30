#ifndef RENDER_H_
#define RENDER_H_

#include "stdio.h"

/**
 * Custom allocator for vulkan objects.
 */
void*
vk_alloc(size_t size) {
    printf("[RENDER] Creating heap memory of size %u", size);
}

/**
 * Create the app info for our application..
 */
VkApplicationInfo
vk_create() {
    VkApplicationInfo appInfo{};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = "starseed";
    appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.pEngineName = "No Engine";
    appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.apiVersion = VK_API_VERSION_1_0;

    return appInfo;
}

/**
 * Create the create info for our application.
 * @param info The application info created earlier.
 */
VkInstanceCreateInfo
vk_create_info(VkApplicationInfo* info)  {
    VkInstanceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo = &appInfo;

    return createInfo;
}



#endif