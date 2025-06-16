#include <stdio.h>

#include "g_game.h"
#include "c_log.h"
#include "r_render.h"

VkResult 
S_CreateDebugUtilsMessengerEXT(
    VkInstance instance, 
    const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, 
    const VkAllocationCallbacks* pAllocator, 
    VkDebugUtilsMessengerEXT* pDebugMessenger
) {
    PFN_vkCreateDebugUtilsMessengerEXT func 
        = (PFN_vkCreateDebugUtilsMessengerEXT) 
            vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
    if (func != NULL) {
        return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
    } else {
        G_Log("ERROR", "Could not find vkCreateDebugUtilsMessengerEXT.");
        return VK_ERROR_EXTENSION_NOT_PRESENT;
    }
}

/* Debugging message callback for vulkan */
VKAPI_ATTR VkBool32 VKAPI_CALL R_CoreVulkanDebugCallback(
    VkDebugUtilsMessageSeverityFlagBitsEXT msg_severity,
    VkDebugUtilsMessageTypeFlagsEXT msg_type,
    const VkDebugUtilsMessengerCallbackDataEXT* callback,
    void* user_data) {

    #define MSG_MAX 4096

    char msg[MSG_MAX] = { 0 };
    
    // Start with the main message
    int offset = snprintf(
        msg, 
        MSG_MAX,
        "%s", 
        callback->pMessage);
    
    // Concatenate object information
    if (callback->objectCount > 0) {
        offset += snprintf(
            msg + offset, 
            MSG_MAX - offset,
            "\n=== Vulkan Objects ===\n"
        );
        for (Uint32 i = 0; i < callback->objectCount && offset < MSG_MAX; i++) {
            const VkDebugUtilsObjectNameInfoEXT* obj = &callback->pObjects[i];
            
            offset += snprintf(msg + offset, MSG_MAX - offset, 
                "Object[%u]: Type=%d, Handle=0x%lx, Name=%s\n",
                i,
                obj->objectType,
                (unsigned long)obj->objectHandle,
                obj->pObjectName ? obj->pObjectName : "unnamed");
        }
    }
    
    // Log the complete message
    if (msg_severity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT) {
        G_Log("VULKAN_ERROR", msg);
    } else {
        G_Log("VULKAN", msg);
    }
    
    return VK_FALSE;
}

void 
S_DestroyDebugUtilsMessengerEXT(
    VkInstance instance, 
    VkDebugUtilsMessengerEXT debugMessenger, 
    const VkAllocationCallbacks* pAllocator
) {
    PFN_vkDestroyDebugUtilsMessengerEXT func 
        = (PFN_vkDestroyDebugUtilsMessengerEXT) 
            vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
    if (func != NULL) {
        func(instance, debugMessenger, pAllocator);
    }
}

void
G_Init(Game* game) {

    G_Log("INFO", "Initializing game.");

    game->running = 0;

    // initialize sdl
    const SDL_InitFlags init_flags = 
        SDL_INIT_VIDEO 
        | SDL_INIT_EVENTS;

    if (SDL_Init(init_flags) != 1) {
        printf("SDL init failed: %s\n", SDL_GetError());
        return;
    }

    // create window
    game->window = window_create(
        "C game",
        800,
        600,
        SDL_WINDOW_VULKAN
    );

    if (game->window.handle == NULL) {
        G_Log("ERROR", "Error creating window.");
        return;
    }

    game->clock = (Clock) { 0 };

    /* Determine validation layers */

    int validation_layers_supported = R_CheckValidationLayerSupport();
    if (enable_validation_layers && !validation_layers_supported) {
        G_Log("ERROR", "Validation layers were enabled but not supported.");
        return;
    }

    /* Initialize vulkan */
    VkApplicationInfo app_info = { 0 };
    app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    app_info.pApplicationName = "cgame";
    app_info.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    app_info.pEngineName = "engine";
    app_info.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    app_info.apiVersion = VK_API_VERSION_1_0;

    // use sdl to get vulkan extensions
    Uint32 count_inst_exts;
    const char* const* inst_exts = SDL_Vulkan_GetInstanceExtensions(
        &count_inst_exts
    );

    if (!inst_exts) {
        G_Log("ERROR", "Error finding sdl vulkan instance extensions.");
        return;
    }

    /* create the vulkan instance */

    int exts_count = count_inst_exts + 1;
    const char** exts = SDL_malloc(exts_count * sizeof(const char*));
    exts[0] = VK_EXT_DEBUG_UTILS_EXTENSION_NAME;
    SDL_memcpy(&exts[1], inst_exts, count_inst_exts * sizeof(const char*));
    // TODO probably not right

    VkInstanceCreateInfo vk_inst_create_info = { 0 };
    vk_inst_create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    vk_inst_create_info.pApplicationInfo = &app_info;
    vk_inst_create_info.enabledExtensionCount = exts_count;
    vk_inst_create_info.ppEnabledExtensionNames = exts;
    if (enable_validation_layers) {
        vk_inst_create_info.enabledLayerCount = CGAME_VALIDATION_LAYERS_COUNT;
        vk_inst_create_info.ppEnabledLayerNames = validation_layers; 
    } else {
        vk_inst_create_info.enabledLayerCount = 0;
    }

    if (vkCreateInstance(&vk_inst_create_info, NULL, &game->vk)) {
        G_Log("ERROR", "Failed to create vulkan instance.");
        return;
    }

    /* Setup debug message callback */

    VkDebugUtilsMessengerCreateInfoEXT debug_create_info = { 0 };
    debug_create_info.sType 
        = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    debug_create_info.messageSeverity 
        = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT 
        | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT 
        | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    debug_create_info.messageType 
        = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT 
        | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT 
        | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    debug_create_info.pfnUserCallback = R_CoreVulkanDebugCallback;
    debug_create_info.pUserData = NULL; // Optional

    // need to fetch the address of the function that creates the debug
    // messenger
    if (S_CreateDebugUtilsMessengerEXT(
        game->vk,
        &debug_create_info,
        NULL,
        &game->debug_messenger
    ) != VK_SUCCESS) {
        G_Log("ERROR", "Failed to create debug messenger.");
        return;
    }
    
    /* Create surface */

    VkSurfaceKHR khr_surface = VK_NULL_HANDLE;
    if (!SDL_Vulkan_CreateSurface(
        game->window.handle, 
        game->vk, 
        NULL, 
        &khr_surface
    )) {
        G_Log("ERROR", "Error creating sdl3 vulkan surface.");
        G_Log("ERROR", SDL_GetError());
        return;
    }

    game->surface = khr_surface;

    /** Get the physical device */

    VkPhysicalDevice* phys_devices = VK_NULL_HANDLE;
    VkPhysicalDevice phys_device = VK_NULL_HANDLE;

    Uint32 device_count = 0;
    vkEnumeratePhysicalDevices(game->vk, &device_count, NULL);

    if (device_count == 0) {
        G_Log("ERROR", "No valid physical graphics device found.");
        return;
    }

    phys_devices = SDL_malloc(device_count * sizeof(VkPhysicalDevice));
    if (!phys_devices) {
        G_Log("ERROR", "Error allocating memory for physical devices array.");
        return;
    }
    vkEnumeratePhysicalDevices(game->vk, &device_count, phys_devices);

    // select the first device based on suitability
    for (Uint32 i = 0; i < device_count; i++) {
        if (R_IsPhysicalDeviceSuitable(phys_devices[i], game->surface)) {
            G_Log("INFO", "Found suitable physical device.");
            phys_device = phys_devices[i];
            break;
        }
    }

    // If this assertion failed, then there is no suitable graphics device.
    if (phys_device == VK_NULL_HANDLE) {
        G_Log("ERROR", "Failed to fetch suitable physical device.");
        return;
    }

    game->gpu = phys_device;

    /* Create logical device */

    queue_family_indices indices = R_FindQueueFamilies(game->gpu, game->surface);

    VkDeviceQueueCreateInfo queue_create_info = { 0 };
    queue_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    // We know atleast the graphics queue exists for this physical device from
    // R_IsPhysicalDeviceSuitable(...)
    queue_create_info.queueFamilyIndex = indices.graphics_family;
    queue_create_info.queueCount = 1;
    float queue_prio = 1.0f; // Takes a pointer for some reason..?
    queue_create_info.pQueuePriorities = &queue_prio;

    /* create presentation queue (for delivering images to the display) */

    VkDeviceQueueCreateInfo* queue_create_infos = NULL;
    // create unique list of queue families since the graphics family and the
    // presentation family may overlap.
    // TODO hardcoded for now. simple set implementation
    Uint32* unique_queue_families = NULL;
    Uint32 unique_queue_families_count = 0;
    if (indices.graphics_family == indices.present_family) {
        unique_queue_families_count = 1;
        // families are the same and one can be used
        unique_queue_families 
            = SDL_malloc(unique_queue_families_count * sizeof(Uint32));

        unique_queue_families[0] = 
            indices.graphics_family | indices.present_family;
    } else {
        unique_queue_families_count = 2;
        // families are NOT the same and both must be added
        unique_queue_families = SDL_malloc(
            unique_queue_families_count * sizeof(Uint32)
        );

        unique_queue_families[0] = 
            indices.graphics_family;
        unique_queue_families[1] =
            indices.present_family;
    }

    queue_create_infos = SDL_malloc(
        unique_queue_families_count * sizeof(VkDeviceQueueCreateInfo)
    );

    float queue_prio2 = 1.0f;
    for (Uint32 i = 0; i < unique_queue_families_count; i++) {
        Uint32 queue_family = unique_queue_families[i];
        VkDeviceQueueCreateInfo info = { 0 };
        info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        info.queueFamilyIndex = queue_family;
        info.queueCount = 1;
        info.pQueuePriorities = &queue_prio2;
        queue_create_infos[i] = info;
    }

    // sets everything to false
    VkPhysicalDeviceFeatures device_feats = { 0 };

    VkDeviceCreateInfo device_create_info = { 0 };
    device_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    device_create_info.pQueueCreateInfos = &queue_create_info;
    device_create_info.queueCreateInfoCount = 1;
    device_create_info.pEnabledFeatures = &device_feats;
    device_create_info.enabledExtensionCount = CGAME_REQURIED_EXTENSIONS;
    device_create_info.ppEnabledExtensionNames = required_exts;
    device_create_info.queueCreateInfoCount = unique_queue_families_count;
    device_create_info.pQueueCreateInfos = queue_create_infos;

    VkDevice device;
    if (vkCreateDevice(
        game->gpu, 
        &device_create_info, 
        NULL, 
        &device
    ) != VK_SUCCESS) {            
        G_Log("ERROR", "Error creating logical device.");
        return;
    }

    game->device = device;

    /* Create vulkan queues */
    vkGetDeviceQueue(game->device, indices.graphics_family, 0, &game->queue);
    vkGetDeviceQueue(game->device, indices.present_family, 0, &game->present_queue);

    /* Create swap chain */

    swapchain_support_details swapchain_support = R_QuerySwapChainSupport(
        game->gpu, game->surface
    );
    VkSurfaceFormatKHR surface_format = R_ChooseSwapSurfaceFormat(
        swapchain_support.formats,
        swapchain_support.formats_count
    );
    VkPresentModeKHR present_mode = R_ChooseSwapPresentMode(
        swapchain_support.present_modes,
        swapchain_support.present_modes_count
    );
    VkExtent2D extent = R_ChooseSwapExtent(
        game->window.handle,
        &swapchain_support.capabilities
    );

    game->swapchain_extent = extent;
    game->swapchain_format = surface_format;

    // get the minimum number of images to keep in the swap chain

    Uint32 min_images = swapchain_support.capabilities.minImageCount + 1;
    if (
        swapchain_support.capabilities.maxImageCount > 0
        && min_images > swapchain_support.capabilities.maxImageCount
    ) {
        min_images = swapchain_support.capabilities.maxImageCount;
    }

    // set game image size
    game->image_size = min_images;

    VkSwapchainCreateInfoKHR swapchain_create_info = { 0 };
    swapchain_create_info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    swapchain_create_info.surface = game->surface;
    swapchain_create_info.minImageCount = min_images;
    swapchain_create_info.imageFormat = surface_format.format;
    swapchain_create_info.imageColorSpace = surface_format.colorSpace;
    swapchain_create_info.imageExtent = extent;
    swapchain_create_info.imageArrayLayers = 1; // always 1 unless creating a
    // stereoscopic 3D application
    swapchain_create_info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    // use VK_IMAGE_USAGE_TRANSFER_DST_BIT for post processing, but the image 
    // will be used as a simple color sequence for displaying.
    
    // configure the swapchain create info based on the current queue families
    Uint32 swapchain_queue_family_indices[2] = {
        indices.graphics_family,
        indices.present_family
    };

    if (indices.graphics_family != indices.present_family) {
        // graphics family and present family are not the same - 2 queue 
        // families
        // VK_SHARING_MODE_CONCURRENT: Images can be used across multiple queue 
        // families without explicit ownership transfers.
        swapchain_create_info.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        swapchain_create_info.queueFamilyIndexCount = 2;
        swapchain_create_info.pQueueFamilyIndices = swapchain_queue_family_indices;
    } else {
        // graphics family and present family are the same
        swapchain_create_info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        // VK_SHARING_MODE_EXCLUSIVE An image is owned by one queue family at a 
        // time and ownership must be explicitly transferred before using it in 
        // another queue family. This option offers the best performance.
        swapchain_create_info.queueFamilyIndexCount = 0; // optional
        swapchain_create_info.pQueueFamilyIndices = NULL; // optional
    }

    // transform the image in the swap chain
    swapchain_create_info.preTransform = 
        swapchain_support.capabilities.currentTransform;
    
    // disable alpha blending with other windows in the window system
    swapchain_create_info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    swapchain_create_info.presentMode = present_mode;
    swapchain_create_info.clipped = VK_TRUE; // click pixels outside the extent
    // when the window gets resized or changed, the swapchain needs to be 
    // recreated.
    swapchain_create_info.oldSwapchain = VK_NULL_HANDLE;

    VkSwapchainKHR swapchain = VK_NULL_HANDLE;
    if (vkCreateSwapchainKHR(
        game->device, 
        &swapchain_create_info, 
        NULL, 
        &swapchain
    ) != VK_SUCCESS) {
        G_Log("ERROR", "Error creating swapchain.");
        return;
    }

    game->swapchain = swapchain;

    /* Create the images buffer for actually rendering to the screen */

    if (vkGetSwapchainImagesKHR(
        device, 
        swapchain, 
        &game->image_size, 
        NULL
    ) != VK_SUCCESS) {
        G_Log("ERROR", "Could not fetch swapchain image size.");
        return;
    }
    game->images = SDL_malloc(game->image_size * sizeof(VkImage));
    if (vkGetSwapchainImagesKHR(
        device, 
        swapchain, 
        &game->image_size, 
        game->images
    ) != VK_SUCCESS) {
        G_Log("ERROR", "Could not fetch swapchain images.");
        return;
    }

    /* create image views */
    game->image_view_size = game->image_size;
    game->image_views = SDL_malloc(game->image_view_size * sizeof(VkImageView));

    for (Uint32 i = 0; i < game->image_view_size; i++) {
        VkImageViewCreateInfo image_view_create_info = { 0 };
        image_view_create_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        image_view_create_info.image = game->images[i];
        // interpret images as 2d
        image_view_create_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
        // swapchain format determines color format
        image_view_create_info.format = game->swapchain_format.format;
        // do not change the color channels in the image view
        image_view_create_info.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
        image_view_create_info.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
        image_view_create_info.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
        image_view_create_info.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
        image_view_create_info.subresourceRange.aspectMask 
            = VK_IMAGE_ASPECT_COLOR_BIT;
        image_view_create_info.subresourceRange.baseMipLevel = 0;
        image_view_create_info.subresourceRange.levelCount = 1;
        image_view_create_info.subresourceRange.baseArrayLayer = 0;
        image_view_create_info.subresourceRange.layerCount = 1;

        // create image view
        if (vkCreateImageView(
            device, 
            &image_view_create_info, 
            NULL, 
            &game->image_views[i]
        ) != VK_SUCCESS) {
            G_Log("ERROR", "Failed to create image view.");
            return;
        }
    }

    /* Create our graphics pipeline */
    if (!R_CreateRenderPass(
        game->device, 
        game->swapchain_format.format,
        &game->render_pass
    )) {
        G_Log("ERROR", "Error creating render pass.");
        return;
    }

    if (!R_CreateGraphicsPipeline(
        game->device,
        game->swapchain_extent,
        game->render_pass,
        &game->pipeline_layout,
        &game->pipeline
    )) {
        G_Log("ERROR", "Error creating graphics pipeline.");
        return;
    }

    /* Create framebuffers */
    game->framebuffer_size = game->image_view_size;
    game->framebuffers = SDL_malloc(game->framebuffer_size * sizeof(VkFramebuffer));
    if (!game->framebuffers) {
        G_Log("ERROR", "Error allocating memory for framebuffers.");
        return;
    }
    for (Uint32 i = 0; i < game->framebuffer_size; i++) {
        // array of image views
        VkImageView attachments[] = {
            game->image_views[i]
        };

        VkFramebufferCreateInfo framebuffer_create_info = { 0 };
        framebuffer_create_info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        framebuffer_create_info.renderPass = game->render_pass;
        framebuffer_create_info.attachmentCount = 1;
        framebuffer_create_info.pAttachments = attachments;
        framebuffer_create_info.width = game->swapchain_extent.width;
        framebuffer_create_info.height = game->swapchain_extent.height;
        framebuffer_create_info.layers = 1;

        if (vkCreateFramebuffer(
            game->device,
            &framebuffer_create_info,
            NULL,
            &game->framebuffers[i]
        ) != VK_SUCCESS) {
            G_Log("ERROR", "Error creating framebuffer.");
            return;
        }
    }
    
    /* Create command pool */

    VkCommandPoolCreateInfo pool_create_info = { 0 };
    pool_create_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    pool_create_info.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    pool_create_info.queueFamilyIndex = indices.graphics_family;

    if (vkCreateCommandPool(
        game->device,
        &pool_create_info,
        NULL,
        &game->command_pool
    ) != VK_SUCCESS) {
        G_Log("ERROR", "Error creating command pool.");
        return;
    }

    /* Create command buffer */

    VkCommandBufferAllocateInfo command_buffer_alloc_info = { 0 };
    command_buffer_alloc_info.sType 
        = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    command_buffer_alloc_info.commandPool = game->command_pool;
    command_buffer_alloc_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    command_buffer_alloc_info.commandBufferCount = 1;

    if (vkAllocateCommandBuffers(
        game->device,
        &command_buffer_alloc_info,
        &game->command_buffer
    ) != VK_SUCCESS) {
        G_Log("ERROR", "Error creating command buffer.");
        return;
    }

    /* create semaphores and fences */
    VkSemaphoreCreateInfo semaphore_info = { 0 };
    semaphore_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
    VkFenceCreateInfo fence_create_info = { 0 };
    fence_create_info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fence_create_info.flags = VK_FENCE_CREATE_SIGNALED_BIT;
    if (vkCreateSemaphore(
        game->device, 
        &semaphore_info, 
        NULL, 
        &game->image_available
    ) != VK_SUCCESS) {
        G_Log("ERROR", "Error creating image available semaphore.");
        return;
    }
    if (vkCreateSemaphore(
        game->device, 
        &semaphore_info, 
        NULL, 
        &game->render_finished
    ) != VK_SUCCESS) {
        G_Log("ERROR", "Error creating render finished semaphore.");
        return;
    }
    if (vkCreateFence(
        game->device, 
        &fence_create_info, 
        NULL, 
        &game->inflight_fence
    ) != VK_SUCCESS) {
        G_Log("ERROR", "Error creating inflight fence.");
        return;
    }

    /* Running is now true */
    game->running = 1;
    
    /* Cleanup */
    SDL_free(unique_queue_families);
    SDL_free(exts);
    SDL_free(phys_devices);
}

void
G_Start(Game* game) {
    while (game->running) {
        /* Update the game clock */
        clock_update(&game->clock);

        /* Render frame */
        R_DrawFrame(
            game->device,
            game->inflight_fence,
            game->swapchain,
            game->image_available,
            game->render_finished,
            game->command_buffer,
            game->queue,
            game->framebuffers,
            game->render_pass,
            game->swapchain_extent,
            game->present_queue,
            game->pipeline
        );

        /** Poll events */
        SDL_Event evt;
        while (SDL_PollEvent(&evt)) {
            if (evt.type == SDL_EVENT_QUIT) {
                G_Stop(game);
            }
        }
    }
}

void 
G_Stop(Game* game) {
    G_Log("INFO", "Stopping game.");

    // wait for device to finish stuff
    vkDeviceWaitIdle(game->device);

    // clean up vulkan stuff

    game->running = 0;

    vkDestroySemaphore(game->device, game->image_available, NULL);
    vkDestroySemaphore(game->device, game->render_finished, NULL);
    vkDestroyFence(game->device, game->inflight_fence, NULL);

    vkDestroyCommandPool(game->device, game->command_pool, NULL);

    for (Uint32 i = 0; i < game->framebuffer_size; i++) {
        vkDestroyFramebuffer(game->device, game->framebuffers[i], NULL);
    }

    vkDestroyPipeline(game->device, game->pipeline, NULL);
    vkDestroyPipelineLayout(game->device, game->pipeline_layout, NULL);
    vkDestroyRenderPass(game->device, game->render_pass, NULL);

    for (Uint32 i = 0; i < game->image_view_size; i++) {
        VkImageView view = game->image_views[i];
        vkDestroyImageView(game->device, view, NULL);
    }

    vkDestroySwapchainKHR(game->device, game->swapchain, NULL);
    vkDestroyDevice(game->device, NULL);

    if (enable_validation_layers) {
        S_DestroyDebugUtilsMessengerEXT(game->vk, game->debug_messenger, NULL);
    }

    vkDestroySurfaceKHR(game->vk, game->surface, NULL);
    vkDestroyInstance(game->vk, NULL);

    window_destroy(&game->window);

    game->running = 0;
}