// r_render.c

#include <SDL3/SDL.h>
#include <string.h>

#define _USE_MATH_DEFINES
#include <math.h>


#include "r_render.h"

#include "c_log.h"
#include "c_utils.h"
#include "g_clock.h"
#include "r_matrix.h"

// defined in r_vulkan.c
extern const int MAX_FRAMES_IN_FLIGHT;

int
R_CreateRenderState(R_RenderState* state) {
    VkResult res = VK_SUCCESS;
    state->vk = (VKH_VulkanState) { 0 };
    state->initialized = 0;

    /* get validation layers */
    int validation_layers_supported = VKH_CheckValidationLayerSupport();
    if (enable_validation_layers) {
        G_Log("INFO", "Validation layers enabled.");
    }
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

    /* create the swapchain + images */
    VKH_CreateSwapchain(
        state->window->handle,
        state->vk.gpu,
        state->vk.device,
        state->vk.surface,
        state->vk.queue_families,
        &state->vk.images,
        &state->vk.swapchain,
        &state->vk.swapchain_format,
        &state->vk.swapchain_extent
    );

    // NOTE: Image Views should always the same size as the Images array.
    state->vk.image_views.size = state->vk.images.size;

    /* create image views */
    VKH_CreateImageViews(
        state->vk.device,
        state->vk.swapchain_format.format,
        &state->vk.images,
        &state->vk.image_views
    );

    /* create render pass */
    VKH_CreateRenderPass(
        state->vk.device,
        state->vk.swapchain_format.format,
        &state->vk.render_pass
    );

    /* set binding description - allocated */
    state->vk.pipeline.vert_input_bind_desc_count = 1;
    state->vk.pipeline.vert_input_bind_desc = SDL_malloc(
        state->vk.pipeline.vert_input_bind_desc_count 
            * sizeof(*state->vk.pipeline.vert_input_bind_desc)
    );
    VKH_GetBindingDescription(
        (Uint32[]) { sizeof(Vertex) }, // stack allocated
        1,
        state->vk.pipeline.vert_input_bind_desc
    );

    /* set attribute description - allocated */
    state->vk.pipeline.vert_input_attrib_desc_count = 2;
    state->vk.pipeline.vert_input_attrib_desc = SDL_malloc(
        state->vk.pipeline.vert_input_attrib_desc_count
            * sizeof(*state->vk.pipeline.vert_input_attrib_desc)
    );
    VKH_GetAttributeDescriptions(
        0,
        state->vk.pipeline.vert_input_attrib_desc_count,
        (Uint32[2]) { offsetof(Vertex, position), offsetof(Vertex, color) },
        (VkFormat[2]) { VK_FORMAT_R32G32_SFLOAT, VK_FORMAT_R32G32B32_SFLOAT  },
        state->vk.pipeline.vert_input_attrib_desc);
    
    /* create descripter set layout before creating the pipeline */
    VKH_CreateDescriptorSetLayout(
        state->vk.device, 
        &state->vk.pipeline.descriptor_layout);

    /* create graphics pipeline */
    VKH_CreateGraphicsPipeline(
        state->vk.device,
        state->vk.swapchain_extent,
        state->vk.render_pass,
        &state->vk.pipeline);

    // NOTE: Framebuffers should always the same size as the Images array.
    state->vk.framebuffers.size = state->vk.images.size;

    /* create framebuffers */
    VKH_CreateFramebuffers(
        state->vk.device,
        state->vk.render_pass,
        state->vk.swapchain_extent,
        state->vk.image_views,
        &state->vk.framebuffers);

    /* Create the command pool */
    VKH_CreateCommandPool(
        state->vk.device,
        state->vk.queue_families,
        &state->vk.pool);

    /* create vertex buffer */
    // Vertex vertices[3] = {
    //     { .position = { .x = 0.0f, .y = -0.5f }, .color = { .x = 1.0f, .y = 0.0f, .z = 0.0f }},
    //     { .position = { .x = 0.5f, .y = 0.5f }, .color = { .x = 0.0f, .y = 1.0f, .z = 0.0f }},
    //     { .position = { .x = 0.5f, .y = 0.5f }, .color = { .x = 0.0f, .y = 0.0f, .z = 1.0f }}
    // };
    Vertex vertices[4] = {
        { .position = { -0.5f, -0.5f }, .color = { 1.0f, 0.0f, 0.0f }},
        { .position = { 0.5f, -0.5f }, .color = { 0.0f, 1.0f, 0.0f }},
        { .position = { 0.5f, 0.5f }, .color = { 0.0f, 0.0f, 1.0f }},
        { .position = { -0.5f, 0.5f }, .color = { 1.0f, 1.0f, 1.0f }}
    };
    Uint32 indices[6] = { 0, 1, 2, 2, 3, 0 };
    VKH_CreateIndexBuffer(state->vk.device,
        state->vk.gpu,
        indices,
        6 * sizeof(*indices),
        state->vk.graphics_queue,
        state->vk.pool,
        &state->vk.index_buffer,
        &state->vk.index_buffer_memory);
    VKH_CreateVertexBuffer(state->vk.device,
        state->vk.gpu,
        vertices,
        4 * sizeof(*vertices),
        state->vk.graphics_queue,
        state->vk.pool,
        &state->vk.vertex_buffer,
        &state->vk.vertex_buffer_memory);
    
    // create uniform buffers after creating the vertex and index buffers
    state->vk.ubo.size = MAX_FRAMES_IN_FLIGHT;
    state->vk.ubo.buffer_list = SDL_calloc(
        state->vk.ubo.size,
        sizeof(*state->vk.ubo.buffer_list));
    state->vk.ubo.mapped = SDL_calloc(
        state->vk.ubo.size,
        sizeof(char));
    state->vk.ubo.memory_list = SDL_calloc(
        state->vk.ubo.size,
        sizeof(*state->vk.ubo.memory_list));
    VKH_CreateUniformBuffers(
        state->vk.device,
        state->vk.gpu,
        &state->vk.ubo);

    // Command buffer size must be equivalent to the MAX FRAMES IN FLIGHT.
    state->vk.command_buffers.size = MAX_FRAMES_IN_FLIGHT;
    state->vk.command_buffers.data = SDL_calloc(state->vk.command_buffers.size,
        sizeof(*state->vk.command_buffers.data));

    /* Create command buffers */
    VKH_CreateCommandBuffer(
        state->vk.device,
        state->vk.pool,
        &state->vk.command_buffers);

    /**
     * Create synchronization objects
     * Each object has a length of MAX_FRAMES_IN_FLIGHT
     * Sync objects are stack allocated
     */
    state->vk.image_available.size = MAX_FRAMES_IN_FLIGHT;
    state->vk.render_finished.size = MAX_FRAMES_IN_FLIGHT;
    state->vk.inflight_fence.size = MAX_FRAMES_IN_FLIGHT;

    for (Uint32 i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {

        /* Create semaphore for images */
        VKH_CreateSemaphore(
            state->vk.device,
            &state->vk.image_available.data[i]);

        /* Create semaphore for finished rendering */
        VKH_CreateSemaphore(
            state->vk.device,
            &state->vk.render_finished.data[i]);

        /* create fence for the host */
        VKH_CreateFence(
            state->vk.device,
            &state->vk.inflight_fence.data[i]);
    }

    if (res == VK_SUCCESS) {
        state->initialized = 1;
    }
    return res;

}

int
R_DestroyRenderState(R_RenderState* state) {
    G_Log("RENDER INFO", "Destroying render state.");

    // cleanup swapchain stuff
    for (Uint32 i = 0; i < state->vk.framebuffers.size; i++) {
        VkFramebuffer framebuffer = state->vk.framebuffers.data[i];
        vkDestroyFramebuffer(
            state->vk.device, 
            framebuffer, 
            NULL);
    }
    for (Uint32 i = 0; i < state->vk.image_views.size; i++) {
        VkImageView view = state->vk.image_views.data[i];
        vkDestroyImageView(state->vk.device, view, NULL);
    }
    vkDestroySwapchainKHR(state->vk.device, state->vk.swapchain, NULL);

    vkDestroyPipeline(state->vk.device, state->vk.pipeline.pipeline, NULL);
    vkDestroyPipelineLayout(
        state->vk.device, state->vk.pipeline.pipeline_layout, NULL);
    vkDestroyRenderPass(state->vk.device, state->vk.render_pass, NULL);

    // destroy uniform buffer list before the descriptor set layout
    for (Uint32 i = 0; i < state->vk.ubo.size; i++) {
        vkDestroyBuffer(state->vk.device, state->vk.ubo.buffer_list[i], NULL);
        vkFreeMemory(state->vk.device, state->vk.ubo.memory_list[i], NULL);
    }

    // cleanup descriptor set layout after swapchain
    vkDestroyDescriptorSetLayout(
        state->vk.device,
        state->vk.pipeline.descriptor_layout,
        NULL);

    // cleanup vertex buffer/index buffer after destroying the swapchain stuff
    vkDestroyBuffer(state->vk.device, state->vk.index_buffer, NULL);
    vkFreeMemory(state->vk.device, state->vk.index_buffer_memory, NULL);

    vkDestroyBuffer(state->vk.device, state->vk.vertex_buffer, NULL);
    vkFreeMemory(state->vk.device, state->vk.vertex_buffer_memory, NULL);

    /* destroy image available semaphores */
    for (Uint32 i = 0; i < state->vk.image_available.size; i++) {
        VkSemaphore semaphore = state->vk.image_available.data[i];
        vkDestroySemaphore(state->vk.device, semaphore, NULL);
    }
    /* destroy render finished semaphores */
    for (Uint32 i = 0; i < state->vk.render_finished.size; i++) {
        VkSemaphore semaphore = state->vk.render_finished.data[i];
        vkDestroySemaphore(state->vk.device, semaphore, NULL);
    }
    /* destroy fence semaphores */
    for (Uint32 i = 0; i < state->vk.inflight_fence.size; i++) {
        VkFence fence = state->vk.inflight_fence.data[i];
        vkDestroyFence(state->vk.device, fence, NULL);
    }
    
    vkDestroyCommandPool(state->vk.device, state->vk.pool, NULL);

    vkDestroyDevice(state->vk.device, NULL);
    
    vkDestroySurfaceKHR(state->vk.instance, state->vk.surface, NULL);
    vkDestroyInstance(state->vk.instance, NULL);

    return 1;
}

void
R_UpdateUniformBuffer(R_RenderState* state, const Clock* clockState) {
    const double time = clockState->currTime;
    R_UniformBufferObject ubo = { 0 };

    ubo.model = M_Mat4Identity();
    Mat4_Rotate(&ubo.model, (vec_t) { .x = 0.0f, .y = 0.0f, .z = 1.0f }, time * M_PI_2);

    ubo.view = Mat4_LookAt(
        (Orientation) { 
            .eye = { .x = 0.0f, .y = 0.0f, .z = 0.0f },
            .right = { .x = 0.0f, .y = 1.0f, .z = 0.0f },
            .up = { .x = 0.0f, .y = 0.0f, .z = 1.0f }
        }
    );

    static const float fov = 60.0f;
    float aspect = state->vk.swapchain_extent.width 
        / (float) state->vk.swapchain_extent.height;
    static float zNear = 0.01f;
    static float zFar = 100.0f;
    ubo.proj = Mat4_PerspectiveProjection(
        fov,
        aspect,
        zNear,
        zFar);

    SDL_memcpy(&state->vk.ubo.mapped[state->current_frame], &ubo, sizeof(ubo));
}

int
R_Draw(R_RenderState* state, const Clock* clockState) {
    // wait for fences
    vkWaitForFences(
        state->vk.device, 
        1, 
        &state->vk.inflight_fence.data[state->current_frame], 
        VK_TRUE, 
        UINT64_MAX);

    Uint32 image_index = 0;
    VkResult result = vkAcquireNextImageKHR(
        state->vk.device,
        state->vk.swapchain,
        UINT64_MAX,
        state->vk.image_available.data[state->current_frame],
        VK_NULL_HANDLE,
        &image_index);

    if (result == VK_ERROR_OUT_OF_DATE_KHR) {
        // recreate the swapchain
        VKH_CreateSwapchain(
            state->window->handle,
            state->vk.gpu,
            state->vk.device,
            state->vk.surface,
            state->vk.queue_families,
            &state->vk.images,
            &state->vk.swapchain,
            &state->vk.swapchain_format,
            &state->vk.swapchain_extent
        );
        return 0;
    } else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
        G_Log("ERROR", "Failed to acquire swapchain image.");
        return result;
    }

    vkResetFences(
        state->vk.device, 
        1, 
        &state->vk.inflight_fence.data[state->current_frame]);

    vkResetCommandBuffer(
        state->vk.command_buffers.data[state->current_frame], 
        0);

    // record command buffer
    VKH_RecordCommandBuffer(
        state->vk.command_buffers.data[state->current_frame],
        image_index,
        state->vk.render_pass,
        state->vk.swapchain_extent,
        state->vk.pipeline.pipeline,
        state->vk.framebuffers,
        state->vk.vertex_buffer,
        state->vk.index_buffer,
        6
    );

    // update uniform buffer
    R_UpdateUniformBuffer(state, clockState);

    // wait semaphores

    VkSubmitInfo submit_info = { 0 };
    submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

    VkSemaphore wait_semaphores[] 
        = { state->vk.image_available.data[state->current_frame] };
    VkPipelineStageFlags wait_stages[] 
        = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
    submit_info.waitSemaphoreCount = 1;
    submit_info.pWaitSemaphores = wait_semaphores;
    submit_info.pWaitDstStageMask = wait_stages;
    submit_info.commandBufferCount = 1;
    submit_info.pCommandBuffers 
        = &state->vk.command_buffers.data[state->current_frame];
    VkSemaphore signal_semaphores[] 
        = { state->vk.render_finished.data[state->current_frame] };
    submit_info.signalSemaphoreCount = 1;
    submit_info.pSignalSemaphores = signal_semaphores;

    if (vkQueueSubmit(
        state->vk.graphics_queue,
        1,
        &submit_info,
        state->vk.inflight_fence.data[state->current_frame]
    ) != VK_SUCCESS) {
        G_Log("ERROR", "Failed to submit draw command buffer.");
        return 0;
    }

    VkPresentInfoKHR present_info = { 0 };
    present_info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    present_info.waitSemaphoreCount = 1;
    present_info.pWaitSemaphores = signal_semaphores;
    VkSwapchainKHR swapchains[] = { state->vk.swapchain };
    present_info.swapchainCount = 1;
    present_info.pSwapchains = swapchains;
    present_info.pImageIndices = &image_index;
    present_info.pResults = NULL; // Optional

    result = vkQueuePresentKHR(state->vk.present_queue, &present_info);

    if (
        result == VK_ERROR_OUT_OF_DATE_KHR 
        || result == VK_SUBOPTIMAL_KHR || state->vk.framebuffer_resized
    ) {
        state->vk.framebuffer_resized = 0;

        // recreate the swapchain
        VKH_CreateSwapchain(
            state->window->handle,
            state->vk.gpu,
            state->vk.device,
            state->vk.surface,
            state->vk.queue_families,
            &state->vk.images,
            &state->vk.swapchain,
            &state->vk.swapchain_format,
            &state->vk.swapchain_extent
        );
    }

    state->current_frame = (state->current_frame + 1) % MAX_FRAMES_IN_FLIGHT;

    return 1;
}