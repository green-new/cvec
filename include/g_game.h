#ifndef GAME_H_
#define GAME_H_

#include <SDL3/SDL.h>
#include <SDL3/SDL_vulkan.h>
#include <vulkan/vulkan.h>

#include "g_window.h"
#include "g_clock.h"

/**
 * Structure containing high-level game information
 */
typedef struct Game {

    Window window;

    Clock clock;

    int running;

    /** The vulkan instance */
    VkInstance vk;

    /** The graphics device */
    VkPhysicalDevice gpu;

    /** The logical device */
    VkDevice device;

    /** Graphics queue. Destroyed when `device` is destroyed */
    VkQueue queue;

    /** The khr surface */
    VkSurfaceKHR surface;

    /* Presentation queue */
    VkQueue present_queue;

    /* Swap chain */
    VkSwapchainKHR swapchain;
    /* Swap chain details */
    VkSurfaceFormatKHR swapchain_format;
    VkExtent2D swapchain_extent;

    /* Images */
    VkImage* images;
    Uint32 image_size;

    /* Image views */
    VkImageView* image_views;
    Uint32 image_view_size;

    /* Shader modules */
    VkShaderModule vert_shader_mod;
    VkShaderModule frag_shader_mod;

    VkPipelineLayout pipeline_layout;

    VkRenderPass render_pass;

    VkPipeline pipeline;

    VkFramebuffer* framebuffers;
    Uint32 framebuffer_size;

    VkCommandPool command_pool;

    VkCommandBuffer command_buffer;

    /* Vulkan thread blocking mechanisms */
    VkSemaphore image_available;
    VkSemaphore render_finished;
    VkFence inflight_fence;
} Game;

/**
 * Initializes the game.
 * @param game The game object.
 */
void
G_Init(Game* game);

/**
 * Starts the game.
 * @param game The game object.
 */
void
G_Start(Game* game);

/**
 * Stops the game.
 * @param game The game object.
 */
void
G_Stop(Game* game);

#endif