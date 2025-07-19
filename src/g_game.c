#include <stdio.h>

#include "g_game.h"
#include "c_log.h"

#define VEC_IMPL_H_
#include "r_render.h"

#include "r_vulkan.h"

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
        "%s\n", 
        callback->pMessage);
    
    // Concatenate object information
    if (callback->objectCount > 0) {
        offset += snprintf(
            msg + offset, 
            MSG_MAX - offset,
            "=== Vulkan Objects ===\n"
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

int
G_Init(game_t* game) {

    G_Log("INFO", "Initializing game.");

    game->running = 0;

    // initialize sdl
    const SDL_InitFlags init_flags = SDL_INIT_VIDEO | SDL_INIT_EVENTS;

    if (SDL_Init(init_flags) != 1) {
        printf("SDL init failed: %s\n", SDL_GetError());
        return 0;
    }

    // create window
    game->window = G_CreateWindow("C game", 800, 600, SDL_WINDOW_VULKAN);

    if (game->window.handle == NULL) {
        G_Log("ERROR", "Error creating window.");
        return 0;
    }

    game->clock = (Clock) { 0 };

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

    /* create render state */
    game->render_state.window = &game->window;
    R_CreateRenderState(&game->render_state);

    // need to fetch the address of the function that creates the debug
    // messenger
    if (S_CreateDebugUtilsMessengerEXT(
        game->render_state.vk.instance,
        &debug_create_info,
        NULL,
        &game->debug_messenger
    ) != VK_SUCCESS) {
        G_Log("ERROR", "Failed to create debug messenger.");
        return 0;
    }

    /* Running is now true */
    game->running = 1;

    return 1;
}

void
G_Start(game_t* game) {
    while (game->running) {
        /* Update the game clock */
        G_ClockUpdate(&game->clock);

        /* Render frame */
        R_Draw(&game->render_state, &game->clock);

        /** Poll events */
        SDL_Event evt;
        while (SDL_PollEvent(&evt)) {
            if (evt.type == SDL_EVENT_QUIT) {
                game->running = 0;
            }
        }
    }
}

void 
G_Stop(game_t* game) {
    G_Log("INFO", "Stopping game.");

    vkDeviceWaitIdle(game->render_state.vk.device);

    if (enable_validation_layers) {
        S_DestroyDebugUtilsMessengerEXT(
            game->render_state.vk.instance, game->debug_messenger, NULL);
    }

    R_DestroyRenderState(&game->render_state);
    G_DestroyWindow(&game->window);
}