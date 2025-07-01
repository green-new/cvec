// r_render.c

#include <SDL3/SDL.h>
#include <string.h>

#define VEC_IMPL_H_
#include "r_render.h"

#include "c_log.h"
#include "c_utils.h"

int
R_Draw(const R_RenderState* state) {
    return VKH_DrawFrame(
        state->vk.device,
        state->vk.inflight_fence,
        state->vk.swapchain,
        state->vk.image_available,
        state->vk.render_finished,
        state->vk.command_buffer,
        state->vk.graphics_queue,
        state->vk.framebuffers,
        state->vk.render_pass,
        state->vk.swapchain_extent,
        state->vk.present_queue,
        state->vk.pipeline.pipeline
    );
}

int
R_CreateRenderState(R_RenderState* state) {
    VkResult res = VK_SUCCESS;
    state->vk = (VKH_VulkanState) { 0 };
    state->initialized = 0;

    /* get validation layers */
    int validation_layers_supported = VKH_CheckValidationLayerSupport();
    if (enable_validation_layers && !validation_layers_supported) {
        G_Log("ERROR", "Validation layers were enabled but not supported.");
        return 0;
    }

    /* create the vulkan instance */
    if (VKH_CreateInstance(&state->vk.instance) != VK_SUCCESS) {
        G_Log("ERROR", "Error creating the Vulkan instance.");
        return 0;
    }

    /* Create surface */
    VKH_CreateSurface(
        state->window->handle,
        state->vk.instance,
        &state->vk.surface
    );

    /* create physical device */
    VKH_CreatePhysicalDevice(
        state->vk.instance,
        state->vk.surface,
        &state->vk.gpu);

    state->vk.queue_families = VKH_FindQueueFamilies(
        state->vk.gpu, state->vk.surface);
    
    VKH_CreateDevice(
        state->vk.gpu,
        state->vk.queue_families,
        &state->vk.device
    );

    /* create queues */
    vkGetDeviceQueue(
        state->vk.device, 
        state->vk.queue_families.graphics_family, 
        0, 
        &state->vk.graphics_queue);

    vkGetDeviceQueue(state->vk.device, 
        state->vk.queue_families.present_family, 
        0, 
        &state->vk.present_queue);

    /* create the swapchain */
    VKH_CreateSwapchain(
        state->window->handle,
        state->vk.gpu,
        state->vk.device,
        state->vk.surface,
        state->vk.queue_families,
        &state->vk.swapchain,
        &state->vk.swapchain_format,
        &state->vk.swapchain_extent
    );

    /* create images */
    VKH_CreateSwapchainImages(
        state->vk.device,
        state->vk.swapchain,
        &state->vk.image_size,
        &state->vk.images
    );

    /* create image views */
    VKH_CreateImageViews(
        state->vk.device,
        state->vk.image_size,
        state->vk.swapchain_format.format,
        state->vk.images,
        &state->vk.image_views
    );

    /* create render pass */
    VKH_CreateRenderPass(
        state->vk.device,
        state->vk.swapchain_format.format,
        &state->vk.render_pass
    );

    /* create graphics pipeline */
    VKH_CreateGraphicsPipeline(
        state->vk.device,
        state->vk.swapchain_extent,
        state->vk.render_pass,
        &state->vk.pipeline.pipeline_layout,
        &state->vk.pipeline.pipeline
    );

    /* create framebuffers */
    VKH_CreateFramebuffers(
        state->vk.device,
        state->vk.image_size,
        state->vk.render_pass,
        state->vk.swapchain_extent,
        state->vk.image_views,
        &state->vk.framebuffers
    );

    /* Create the command pool */
    VKH_CreateCommandPool(
        state->vk.device,
        state->vk.queue_families,
        &state->vk.pool
    );

    /* Create command buffer */
    VKH_CreateCommandBuffer(
        state->vk.device,
        state->vk.pool,
        &state->vk.command_buffer
    );

    /* Create semaphore for images */
    VKH_CreateSemaphore(
        state->vk.device,
        &state->vk.image_available
    );

    /* Create semaphore for finished rendering */
    VKH_CreateSemaphore(
        state->vk.device,
        &state->vk.render_finished
    );

    /* create fence for the host */
    VKH_CreateFence(
        state->vk.device,
        &state->vk.inflight_fence
    );

    if (res == VK_SUCCESS) {
        state->initialized = 1;
    }
    return res;

}

int
R_DestroyRenderState(R_RenderState* state) {
    G_Log("RENDER INFO", "Destroying render state.");

    // wait for device to finish stuff
    vkDeviceWaitIdle(state->vk.device);

    vkDestroySemaphore(state->vk.device, state->vk.image_available, NULL);
    vkDestroySemaphore(state->vk.device, state->vk.render_finished, NULL);
    vkDestroyFence(state->vk.device, state->vk.inflight_fence, NULL);

    vkDestroyCommandPool(state->vk.device, state->vk.pool, NULL);

    for (Uint32 i = 0; i < state->vk.framebuffer_size; i++) {
        vkDestroyFramebuffer(state->vk.device, state->vk.framebuffers[i], NULL);
    }

    vkDestroyPipeline(state->vk.device, state->vk.pipeline.pipeline, NULL);
    vkDestroyPipelineLayout(
        state->vk.device, state->vk.pipeline.pipeline_layout, NULL);

    vkDestroyRenderPass(state->vk.device, state->vk.render_pass, NULL);

    for (Uint32 i = 0; i < state->vk.image_size; i++) {
        VkImageView view = state->vk.image_views[i];
        vkDestroyImageView(state->vk.device, view, NULL);
    } 

    vkDestroySwapchainKHR(state->vk.device, state->vk.swapchain, NULL);
    vkDestroyDevice(state->vk.device, NULL);

    vkDestroySurfaceKHR(state->vk.instance, state->vk.surface, NULL);
    vkDestroyInstance(state->vk.instance, NULL);

    return 1;
}