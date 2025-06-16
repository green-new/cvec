#include "r_render.h"
#include "c_log.h"
#include "c_utils.h"

#include <string.h>

const char* required_exts[CGAME_REQURIED_EXTENSIONS] = {
    VK_KHR_SWAPCHAIN_EXTENSION_NAME
};

const VkDynamicState dynamic_states[CGAME_DYNAMIC_STATES_COUNT] = {
    VK_DYNAMIC_STATE_VIEWPORT,
    VK_DYNAMIC_STATE_SCISSOR
};

const char* validation_layers[CGAME_VALIDATION_LAYERS_COUNT] = {
    "VK_LAYER_KHRONOS_validation"
};

#ifdef NDEBUG
    const int enable_validation_layers = 1;
#else
    const int enable_validation_layers = 0;
#endif

VkSurfaceFormatKHR
R_ChooseSwapSurfaceFormat(const VkSurfaceFormatKHR* formats, Uint32 size) {
    static VkFormat acceptable_format = VK_FORMAT_B8G8R8A8_SRGB;
    static VkColorSpaceKHR acceptable_color_space 
        = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;
    for (Uint32 i = 0; i < size; i++) {
        VkSurfaceFormatKHR format = formats[i];
        if (
            format.format == acceptable_format 
            && format.colorSpace == acceptable_color_space
        ) {
            return format;
        }
    }

    return formats[0];
}

VkPresentModeKHR
R_ChooseSwapPresentMode(const VkPresentModeKHR* present_modes, Uint32 size) {
    static VkPresentModeKHR acceptable_mode = VK_PRESENT_MODE_MAILBOX_KHR;
    for (Uint32 i = 0; i < size; i++) {
        VkPresentModeKHR mode = present_modes[i];
        if (mode == acceptable_mode) {
            return mode;
        }
    }
    // fallback on FIFO
    return VK_PRESENT_MODE_FIFO_KHR;
}


VkExtent2D 
R_ChooseSwapExtent(
    SDL_Window* window, 
    const VkSurfaceCapabilitiesKHR* capabilities
) {
    if (capabilities->currentExtent.width != UINT32_MAX) {
        return capabilities->currentExtent;
    } else {

        // get real pixel size of the window due to high pixel densities
        int width, height;
        SDL_GetWindowSize(window, &width, &height);

        VkExtent2D actual_extent = { 0 };
        actual_extent.height = height;
        actual_extent.width = width;

        actual_extent.width = SDL_clamp(
            actual_extent.width, 
            capabilities->minImageExtent.width, 
            capabilities->maxImageExtent.width
        );

        actual_extent.height = SDL_clamp(
            actual_extent.height,
            capabilities->minImageExtent.height,
            capabilities->maxImageExtent.height
        );

        return actual_extent;
    }
}

swapchain_support_details
R_QuerySwapChainSupport(VkPhysicalDevice device, VkSurfaceKHR surface) {
    swapchain_support_details details = { 0 };

    // get the surface capabilities

    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(
        device, 
        surface, 
        &details.capabilities);

    // get surface formats

    vkGetPhysicalDeviceSurfaceFormatsKHR(
        device, 
        surface, 
        &details.formats_count, 
        NULL);
    details.formats 
        = SDL_malloc(details.formats_count * sizeof(VkSurfaceFormatKHR));
    vkGetPhysicalDeviceSurfaceFormatsKHR(
        device, 
        surface, 
        &details.formats_count, 
        details.formats);

    // get present modes

    vkGetPhysicalDeviceSurfacePresentModesKHR(
        device, 
        surface, 
        &details.present_modes_count, 
        NULL);
    
    details.present_modes 
        = SDL_malloc(details.present_modes_count * sizeof(VkPresentModeKHR));

    vkGetPhysicalDeviceSurfacePresentModesKHR(
        device, 
        surface, 
        &details.present_modes_count, 
        details.present_modes);


    return details;
}

int 
R_CheckDeviceExtensionSupport(VkPhysicalDevice device) {
    Uint32 ext_count;
    vkEnumerateDeviceExtensionProperties(device, NULL, &ext_count, NULL);

    // get the available extensions for this device

    VkExtensionProperties* available_exts = SDL_malloc(
        ext_count * sizeof(VkExtensionProperties)
    );
    vkEnumerateDeviceExtensionProperties(device, NULL, &ext_count, available_exts);

    // loop through the available extensions and compare them to the list of
    // required extensions.

    int fulfilled = CGAME_REQURIED_EXTENSIONS;

    for (Uint32 i = 0; i < CGAME_REQURIED_EXTENSIONS; i++) {
        const char* required_ext = required_exts[i];
        for (Uint32 j = 0; j < ext_count; j++) {
            VkExtensionProperties device_ext = available_exts[j];
            if (strcmp(required_ext, device_ext.extensionName) == 0) {
                fulfilled--;
                break; // goto the next iteration
            }
        }
    }

    // if all required extensions were met, then pass
    return fulfilled == 0;
} 

int
R_IsPhysicalDeviceSuitable(VkPhysicalDevice device, VkSurfaceKHR surface) {
    VkPhysicalDeviceProperties device_props;
    VkPhysicalDeviceFeatures device_feats;
    vkGetPhysicalDeviceProperties(device, &device_props);
    vkGetPhysicalDeviceFeatures(device, &device_feats);

    // test and see if we have any queue families
    
    queue_family_indices indices = R_FindQueueFamilies(device, surface);

    // test for a list of device extensions

    int exts_supported = R_CheckDeviceExtensionSupport(device);

    // test swap chain support

    int swapchain_good = 0;

    if (exts_supported) {
        G_Log("INFO", "Defined extensions supported.");
        swapchain_support_details swapchain_support = R_QuerySwapChainSupport(
            device, surface
        );

        // must support formats and present modes

        swapchain_good = swapchain_support.formats_count != 0
            && swapchain_support.present_modes_count != 0
            && swapchain_support.formats != NULL
            && swapchain_support.present_modes != NULL;
    }

    return swapchain_good
        && exts_supported
        && !indices.none_graphics_family
        && !indices.none_present_family
        && device_props.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU 
        && device_feats.geometryShader;
}

queue_family_indices
R_FindQueueFamilies(VkPhysicalDevice device, VkSurfaceKHR surface) {

    queue_family_indices indices = { 0 };
    indices.none_graphics_family = 1;
    indices.none_present_family = 1;

    // get graphics family

    Uint32 family_count = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(device, &family_count, NULL);

    // graphics device can have multiple queue families

    VkQueueFamilyProperties* queue_families 
        = SDL_malloc(family_count * sizeof(VkQueueFamilyProperties));

    vkGetPhysicalDeviceQueueFamilyProperties(
        device, 
        &family_count, 
        queue_families);

    for (Uint32 i = 0; i < family_count; i++) {
        const VkQueueFamilyProperties props = queue_families[i];

        VkBool32 supported = false;
        vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &supported);

        if (supported) {
            indices.none_present_family = 0;
            indices.present_family = i;
        }

        // see if the graphics device supports the graphics queue
        if (props.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
            indices.none_graphics_family = 0;
            indices.graphics_family = i;
        }
    }

    return indices;
}

int
R_CreateGraphicsPipeline(
    VkDevice device,
    VkExtent2D extent,
    VkRenderPass render_pass,
    VkPipelineLayout* out_layout,
    VkPipeline* out_pipeline
) {
    buffer vert_shader = { 0 }, frag_shader = { 0 };
    
    if (!C_ReadBinaryFile("./shader/vert.spv", &vert_shader)) {
        G_Log("ERROR", "Failed to read vert shader.");
        return 0;
    }
    if (!C_ReadBinaryFile("./shader/frag.spv", &frag_shader)) {
        G_Log("ERROR", "Failed to read vert shader.");
        return 0;
    }

    VkShaderModule vert_shader_mod = R_CreateShaderModule(device, &vert_shader);
    VkShaderModule frag_shader_mod = R_CreateShaderModule(device, &frag_shader);

    if (
        vert_shader_mod == VK_NULL_HANDLE 
        || frag_shader_mod == VK_NULL_HANDLE
    ) {
        G_Log("ERROR", "Failed to create one of the shader modules.");
        return 0;
    }
    // vertex shader
    VkPipelineShaderStageCreateInfo vert_create_info = { 0 };
    vert_create_info.sType 
        = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    vert_create_info.stage = VK_SHADER_STAGE_VERTEX_BIT;
    vert_create_info.module = vert_shader_mod;
    vert_create_info.pName = "main";
    
    // fragment shader
    VkPipelineShaderStageCreateInfo frag_create_info = { 0 };
    frag_create_info.sType 
        = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    frag_create_info.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    frag_create_info.module = frag_shader_mod;
    frag_create_info.pName = "main";

    // prepare the shader stage pipeline
    VkPipelineShaderStageCreateInfo shader_stages[] = {
        vert_create_info,
        frag_create_info
    };

    // vertex input
    VkPipelineVertexInputStateCreateInfo vertex_input_create_info = { 0 };
    vertex_input_create_info.sType 
        = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vertex_input_create_info.vertexBindingDescriptionCount = 0;
    vertex_input_create_info.pVertexBindingDescriptions = NULL; 
    vertex_input_create_info.vertexAttributeDescriptionCount = 0;
    vertex_input_create_info.pVertexAttributeDescriptions = NULL; 

    // create the input assembly for geometry
    VkPipelineInputAssemblyStateCreateInfo input_assembly = { 0 };
    input_assembly.sType 
        = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    input_assembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    input_assembly.primitiveRestartEnable = VK_FALSE;

    // viewports and scissors
    VkViewport viewport = { 0 };
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = (float) extent.width;
    viewport.height = (float) extent.height;
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;

    VkRect2D scissor = { 0 };
    scissor.offset = (VkOffset2D) { 0, 0 };
    scissor.extent = extent;

    // create the dynamic state
    VkPipelineDynamicStateCreateInfo dyn_state_create_info = { 0 };
    dyn_state_create_info.sType 
        = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    dyn_state_create_info.dynamicStateCount = CGAME_DYNAMIC_STATES_COUNT;
    dyn_state_create_info.pDynamicStates = dynamic_states;

    VkPipelineViewportStateCreateInfo viewport_state_create_info = { 0 };
    viewport_state_create_info.sType
        = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewport_state_create_info.viewportCount = 1;
    viewport_state_create_info.pViewports = &viewport;
    viewport_state_create_info.scissorCount = 1;
    viewport_state_create_info.pScissors = &scissor;

    VkPipelineRasterizationStateCreateInfo raster_create_info = { 0 };
    raster_create_info.sType 
        = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    raster_create_info.depthClampEnable = VK_FALSE; // requires gpu feature 
    // being enabled
    raster_create_info.rasterizerDiscardEnable = VK_FALSE;
    raster_create_info.polygonMode = VK_POLYGON_MODE_FILL;
    // VK_POLYGON_MODE_FILL: fill the area of the polygon with fragments
    // VK_POLYGON_MODE_LINE: polygon edges are drawn as lines
    // VK_POLYGON_MODE_POINT: polygon vertices are drawn as points
    raster_create_info.lineWidth = 1.0f;
    raster_create_info.cullMode = VK_CULL_MODE_BACK_BIT; // cull back faces
    raster_create_info.frontFace = VK_FRONT_FACE_CLOCKWISE; // vertex order
    // depth bias usually used in shadow maps
    raster_create_info.depthBiasEnable = VK_FALSE;
    raster_create_info.depthBiasConstantFactor = 0.0f; 
    raster_create_info.depthBiasClamp = 0.0f; 
    raster_create_info.depthBiasSlopeFactor = 0.0f; 

    // multisampling, disabled. revisit later
    VkPipelineMultisampleStateCreateInfo multisampling = { 0 };
    multisampling.sType 
        = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multisampling.sampleShadingEnable = VK_FALSE;
    multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
    multisampling.minSampleShading = 1.0f;
    multisampling.pSampleMask = NULL;
    multisampling.alphaToCoverageEnable = VK_FALSE;
    multisampling.alphaToOneEnable = VK_FALSE;

    // no depth and stencil testing for now

    // color blending phase
    // we only have one framebuffer
    VkPipelineColorBlendAttachmentState color_blend_att_create_info = { 0 };
    color_blend_att_create_info.colorWriteMask = VK_COLOR_COMPONENT_R_BIT 
        | VK_COLOR_COMPONENT_G_BIT 
        | VK_COLOR_COMPONENT_B_BIT 
        | VK_COLOR_COMPONENT_A_BIT;
    color_blend_att_create_info.blendEnable = VK_FALSE;
    color_blend_att_create_info.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;
    color_blend_att_create_info.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;
    color_blend_att_create_info.colorBlendOp = VK_BLEND_OP_ADD;
    color_blend_att_create_info.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
    color_blend_att_create_info.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
    color_blend_att_create_info.alphaBlendOp = VK_BLEND_OP_ADD;

    // color blend state
    VkPipelineColorBlendStateCreateInfo color_blend_st_create_info = { 0 };
    color_blend_st_create_info.sType 
        = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    color_blend_st_create_info.logicOpEnable = VK_FALSE;
    color_blend_st_create_info.logicOp = VK_LOGIC_OP_COPY; 
    color_blend_st_create_info.attachmentCount = 1;
    color_blend_st_create_info.pAttachments = &color_blend_att_create_info;
    color_blend_st_create_info.blendConstants[0] = 0.0f; 
    color_blend_st_create_info.blendConstants[1] = 0.0f; 
    color_blend_st_create_info.blendConstants[2] = 0.0f; 
    color_blend_st_create_info.blendConstants[3] = 0.0f; 

    // create the pipeline layout
    VkPipelineLayoutCreateInfo pipeline_layout_create_info = { 0 };
    pipeline_layout_create_info.sType 
        = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipeline_layout_create_info.setLayoutCount = 0; 
    pipeline_layout_create_info.pSetLayouts = NULL; 
    pipeline_layout_create_info.pushConstantRangeCount = 0; 
    pipeline_layout_create_info.pPushConstantRanges = NULL;

    if (vkCreatePipelineLayout(
        device, 
        &pipeline_layout_create_info, 
        NULL, 
        out_layout
    ) != VK_SUCCESS) {
        G_Log("ERROR", "Failed to create pipeline layout.");
        return 0;
    }

    VkGraphicsPipelineCreateInfo pipeline_create_info = { 0 };
    pipeline_create_info.sType 
        = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipeline_create_info.stageCount = 2;
    pipeline_create_info.pStages = shader_stages;
    pipeline_create_info.pVertexInputState = &vertex_input_create_info;
    pipeline_create_info.pInputAssemblyState = &input_assembly;
    pipeline_create_info.pViewportState = &viewport_state_create_info;
    pipeline_create_info.pRasterizationState = &raster_create_info;
    pipeline_create_info.pMultisampleState = &multisampling;
    pipeline_create_info.pDepthStencilState = NULL; // we will do this later
    pipeline_create_info.pColorBlendState = &color_blend_st_create_info;
    pipeline_create_info.pDynamicState = &dyn_state_create_info;
    pipeline_create_info.layout = *out_layout;
    pipeline_create_info.renderPass = render_pass;
    pipeline_create_info.subpass = 0;
    // base pipelines are optional - allows you to create pipelines based on
    // preexisting pipelines
    pipeline_create_info.basePipelineHandle = VK_NULL_HANDLE;
    pipeline_create_info.basePipelineIndex = -1;

    if (vkCreateGraphicsPipelines(
        device,
        VK_NULL_HANDLE,
        1,
        &pipeline_create_info,
        NULL,
        out_pipeline
    ) != VK_SUCCESS) {
        return 0;
    }

    // free shader data
    C_FreeFileBuffer(&vert_shader);
    C_FreeFileBuffer(&frag_shader);

    // safe to delete shader modules now
    vkDestroyShaderModule(device, vert_shader_mod, NULL);
    vkDestroyShaderModule(device, frag_shader_mod, NULL);

    return 1;
}

VkShaderModule 
R_CreateShaderModule(VkDevice device, const buffer* binary) {
    // binary was not the right size
    if (binary->size % 4 != 0) {
        G_Log("ERROR", "Shader binary size was not a multiple of 4.");
        return VK_NULL_HANDLE;
    }
    VkShaderModuleCreateInfo create_info = { 0 };
    create_info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    create_info.codeSize = binary->size;
    create_info.pCode = (const Uint32*) binary->data; // cast to uint32 pointer
    // technically undefined behavior, probably?
    VkShaderModule module;
    
    if (vkCreateShaderModule(
        device, 
        &create_info, 
        NULL, 
        &module
    ) != VK_SUCCESS) {
        G_Log("ERROR", "Failed to create shader module.");
        return VK_NULL_HANDLE;
    }

    return module;
}

int
R_CreateRenderPass(
    VkDevice device, 
    VkFormat image_format, 
    VkRenderPass* render_pass
) {
    VkAttachmentDescription color_attach = { 0 };
    color_attach.format = image_format;
    color_attach.samples = VK_SAMPLE_COUNT_1_BIT;
    // clear values after each frame and store frame memory afterwards
    color_attach.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    color_attach.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    // we do nothing with stencil data
    color_attach.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    color_attach.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    // we use the swap chain to render the image
    color_attach.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    color_attach.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

    // for subpasses
    VkAttachmentReference color_attach_ref = { 0 };
    color_attach_ref.attachment = 0;
    color_attach_ref.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkSubpassDescription subpass = { 0 };
    // this is a graphics subpass since compute subpasses may exist in the 
    // future for Vulkan (now)
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = &color_attach_ref;

    VkSubpassDependency dependency = { 0 };
    dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
    dependency.dstSubpass = 0;
    dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.srcAccessMask = 0;
    dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

    VkRenderPassCreateInfo render_pass_create_info = { 0 };
    render_pass_create_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    render_pass_create_info.attachmentCount = 1;
    render_pass_create_info.pAttachments = &color_attach;
    render_pass_create_info.subpassCount = 1;
    render_pass_create_info.pSubpasses = &subpass;
    render_pass_create_info.dependencyCount = 1;
    render_pass_create_info.pDependencies = &dependency;
    

    if (vkCreateRenderPass(
        device, 
        &render_pass_create_info, 
        NULL, 
        render_pass
    ) != VK_SUCCESS) {
        return 0;
    }

    return 1;
}

int
R_RecordCommandBuffer(
    VkCommandBuffer buffer,
    VkFramebuffer* framebuffers,
    Uint32 image_index,
    VkRenderPass render_pass,
    VkExtent2D extent,
    VkPipeline pipeline
) {
    VkCommandBufferBeginInfo buffer_begin_info = { 0 };
    buffer_begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    buffer_begin_info.flags = 0;
    buffer_begin_info.pInheritanceInfo = NULL;

    if (vkBeginCommandBuffer(buffer, &buffer_begin_info) != VK_SUCCESS) {
        G_Log("ERROR", "Error begin recording command buffer.");
        return 0;
    }

    // drawing starts by beginning the render pass.
    VkRenderPassBeginInfo render_pass_info = { 0 };
    render_pass_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    render_pass_info.renderPass = render_pass;
    render_pass_info.framebuffer = framebuffers[image_index];
    render_pass_info.renderArea.offset = (VkOffset2D) { 0, 0 };
    render_pass_info.renderArea.extent = extent;

    // using black for the clear color with 100% opacity
    VkClearValue clearColor = (VkClearValue) {{{0.0f, 0.0f, 0.0f, 1.0f}}};
    render_pass_info.clearValueCount = 1;
    render_pass_info.pClearValues = &clearColor;

    vkCmdBeginRenderPass(buffer, &render_pass_info, VK_SUBPASS_CONTENTS_INLINE);
    vkCmdBindPipeline(buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);

    // the viewport and scissor state for the pipeline we created was made to be
    // dynamic, so we need to pass it to the command buffer
    VkViewport viewport = { 0 };
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = (float) extent.width;
    viewport.height = (float) extent.height;
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;
    vkCmdSetViewport(buffer, 0, 1, &viewport);

    VkRect2D scissor = { 0 };
    scissor.offset = (VkOffset2D) { 0, 0 };
    scissor.extent = extent;
    vkCmdSetScissor(buffer, 0, 1, &scissor);

    // issue draw command
    vkCmdDraw(buffer, 3, 1, 0, 0);

    vkCmdEndRenderPass(buffer);

    if (vkEndCommandBuffer(buffer) != VK_SUCCESS) {
        G_Log("ERROR", "Error end recording command buffer.");
        return 0;
    }

    return 1;

}

int
R_DrawFrame(
    VkDevice device,
    VkFence fence,
    VkSwapchainKHR swapchain,
    VkSemaphore image_available,
    VkSemaphore render_finished,
    VkCommandBuffer buffer,
    VkQueue graphics_queue,
    VkFramebuffer* framebuffers,
    VkRenderPass render_pass,
    VkExtent2D extent,
    VkQueue present_queue,
    VkPipeline pipeline
) {
    vkWaitForFences(device, 1, &fence, VK_TRUE, UINT64_MAX);
    vkResetFences(device, 1, &fence);

    Uint32 image_index = 0;
    vkAcquireNextImageKHR(
        device, 
        swapchain, 
        UINT64_MAX, 
        image_available, 
        VK_NULL_HANDLE, 
        &image_index);
    vkResetCommandBuffer(buffer, 0);

    // record command buffer
    R_RecordCommandBuffer(
        buffer,
        framebuffers,
        image_index,
        render_pass,
        extent,
        pipeline
    );

    VkSubmitInfo submit_info = { 0 };
    submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

    VkSemaphore wait_semaphores[] = { image_available };
    VkPipelineStageFlags wait_stages[] 
        = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
    submit_info.waitSemaphoreCount = 1;
    submit_info.pWaitSemaphores = wait_semaphores;
    submit_info.pWaitDstStageMask = wait_stages;
    submit_info.commandBufferCount = 1;
    submit_info.pCommandBuffers = &buffer;
    VkSemaphore signal_semaphores[] = { render_finished };
    submit_info.signalSemaphoreCount = 1;
    submit_info.pSignalSemaphores = signal_semaphores;

    if (vkQueueSubmit(
        graphics_queue,
        1,
        &submit_info,
        fence
    ) != VK_SUCCESS) {
        G_Log("ERROR", "Failed to submit draw command buffer.");
        return 0;
    }

    VkPresentInfoKHR present_info = { 0 };
    present_info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    present_info.waitSemaphoreCount = 1;
    present_info.pWaitSemaphores = signal_semaphores;
    VkSwapchainKHR swapchains[] = { swapchain };
    present_info.swapchainCount = 1;
    present_info.pSwapchains = swapchains;
    present_info.pImageIndices = &image_index;
    present_info.pResults = NULL; // Optional
    vkQueuePresentKHR(present_queue, &present_info);

    return 1;
}

int
R_CheckValidationLayerSupport() {
    Uint32 layer_count = 0;
    vkEnumerateInstanceLayerProperties(&layer_count, NULL);

    VkLayerProperties* props 
        = SDL_malloc(layer_count * sizeof(VkLayerProperties));

    vkEnumerateInstanceLayerProperties(&layer_count, props);

    for (Uint32 i = 0; i < CGAME_VALIDATION_LAYERS_COUNT; i++) {
        const char* layer_name = validation_layers[i];
        int found = 0;

        for (Uint32 j = 0; j < layer_count; j++) {
            VkLayerProperties prop = props[j];
            if (strcmp(layer_name, prop.layerName) == 0) {
                found = 1;
                break;
            }
        }

        if (!found) {
            return 0;
        }
    }

    return 1;
}