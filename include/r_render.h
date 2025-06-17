/**
 * File: VKH_render.h
 * Description: Contains code defintitions for the rendering part of the game.
 * Author: green
 */
#ifndef RENDEVKH_H_
#define RENDEVKH_H_

#include <stdio.h>
#include <stdlib.h>

#include "vulkan/vulkan.h"
#include "SDL3/SDL.h"

#include "c_utils.h"
#include "g_window.h"
#include "r_vulkan.h"

#define NDEBUG 1 // are we debug mode?

typedef struct {
    VKH_VulkanInstance* vulkan_instance;

    VkFramebuffer* framebuffers;
    Uint32 framebuffer_size;

    VkRenderPass render_pass;

    VkCommandPool command_pool;
    VkCommandBuffer command_buffer;

    VKH_GraphicsPipeline* pipeline;

    VkSemaphore image_available;
    VkSemaphore render_finished;
    VkFence inflight_fence;
} R_RenderState;



#endif