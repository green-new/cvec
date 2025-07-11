#include <SDL3/SDL_vulkan.h>

#include "r_vulkan.h"
#include "c_log.h"

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

#define NDEBUG 1

#ifdef NDEBUG
    const int enable_validation_layers = 1;
#else
    const int enable_validation_layers = 0;
#endif

const int MAX_FRAMES_IN_FLIGHT = 2;

VkResult
VKH_CreateInstance(VkInstance* vk) {
    VkResult res = VK_SUCCESS;

    VkApplicationInfo app_info = { 0 };
    app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    app_info.pApplicationName = "cgame";
    app_info.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    app_info.pEngineName = "engine";
    app_info.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    app_info.apiVersion = VK_API_VERSION_1_0;

    Uint32 count_inst_exts;
    const char* const* inst_exts = SDL_Vulkan_GetInstanceExtensions(
        &count_inst_exts
    );

    if (!inst_exts) {
        G_Log("ERROR", "Error finding SDL Vulkan instance extensions.");
        return 0;
    }
    int exts_count = count_inst_exts + 1;
    
    const char** exts = SDL_malloc(exts_count * sizeof(const char*));
    if (!exts) {
        G_Log("ERROR", "Failed to allocate memory for instance extensions.");
        return 0;
    }

    exts[0] = VK_EXT_DEBUG_UTILS_EXTENSION_NAME;
    SDL_memcpy(&exts[1], inst_exts, count_inst_exts * sizeof(const char*));

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

    if ((res = vkCreateInstance(&vk_inst_create_info, NULL, vk))
        != VK_SUCCESS) {
        G_Log("ERROR", "Failed to create vulkan instance.");
        SDL_free(exts);
        return res;
    }

    return res;
}

VkSurfaceFormatKHR
VKH_ChooseSwapSurfaceFormat(const VkSurfaceFormatKHR* formats, Uint32 size) {
    VkFormat acceptable_format = VK_FORMAT_B8G8R8A8_SRGB;
    VkColorSpaceKHR acceptable_color_space 
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
VKH_ChooseSwapPresentMode(const VkPresentModeKHR* present_modes, Uint32 size) {
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
VKH_ChooseSwapExtent(
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

VKH_SwapchainSupportDetails
VKH_QuerySwapChainSupport(VkPhysicalDevice device, VkSurfaceKHR surface) {
    VKH_SwapchainSupportDetails details = { 0 };

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
VKH_CheckDeviceExtensionSupport(VkPhysicalDevice device) {
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
VKH_IsPhysicalDeviceSuitable(VkPhysicalDevice device, VkSurfaceKHR surface) {
    VkPhysicalDeviceProperties device_props;
    VkPhysicalDeviceFeatures device_feats;
    vkGetPhysicalDeviceProperties(device, &device_props);
    vkGetPhysicalDeviceFeatures(device, &device_feats);

    // test and see if we have any queue families
    
    VKH_QueueFamilyIndices indices = VKH_FindQueueFamilies(device, surface);

    // test for a list of device extensions

    int exts_supported = VKH_CheckDeviceExtensionSupport(device);

    // test swap chain support

    int swapchain_good = 0;

    if (exts_supported) {
        G_Log("INFO", "Defined extensions supported.");
        VKH_SwapchainSupportDetails swapchain_support = 
            VKH_QuerySwapChainSupport(device, surface);

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

VKH_QueueFamilyIndices
VKH_FindQueueFamilies(VkPhysicalDevice device, VkSurfaceKHR surface) {
    VKH_QueueFamilyIndices indices = { 0 };
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

VkResult
VKH_CreateGraphicsPipeline(
    VkDevice device,
    VkExtent2D extent,
    VkRenderPass render_pass,
    VKH_GraphicsPipeline* pipeline
) {
    VkResult res = VK_SUCCESS;
    buffer vert_shader = { 0 }, frag_shader = { 0 };
    
    if (!C_ReadBinaryFile("./shader/shader.vert.spv", &vert_shader)) {
        G_Log("ERROR", "Failed to read vert shader.");
        return -1;
    }
    if (!C_ReadBinaryFile("./shader/shader.frag.spv", &frag_shader)) {
        G_Log("ERROR", "Failed to read vert shader.");
        return -1;
    }

    VkShaderModule vert_shader_mod = VKH_CreateShaderModule(device, &vert_shader);
    VkShaderModule frag_shader_mod = VKH_CreateShaderModule(device, &frag_shader);

    if (
        vert_shader_mod == VK_NULL_HANDLE 
        || frag_shader_mod == VK_NULL_HANDLE
    ) {
        G_Log("ERROR", "Failed to create one of the shader modules.");
        return -1;
    }
    // vertex shader
    VkPipelineShaderStageCreateInfo vrtx_shdr_ci = { 0 };
    vrtx_shdr_ci.sType 
        = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    vrtx_shdr_ci.stage = VK_SHADER_STAGE_VERTEX_BIT;
    vrtx_shdr_ci.module = vert_shader_mod;
    vrtx_shdr_ci.pName = "main";
    
    // fragment shader
    VkPipelineShaderStageCreateInfo frag_shdr_ci = { 0 };
    frag_shdr_ci.sType 
        = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    frag_shdr_ci.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    frag_shdr_ci.module = frag_shader_mod;
    frag_shdr_ci.pName = "main";

    // prepare the shader stage pipeline
    VkPipelineShaderStageCreateInfo shader_stages[] = {
        vrtx_shdr_ci,
        frag_shdr_ci
    };

    // vertex input
    VkPipelineVertexInputStateCreateInfo vrtx_in_ci = { 0 };
    vrtx_in_ci.sType 
        = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vrtx_in_ci.vertexBindingDescriptionCount = pipeline->vert_input_bind_desc_count;
    vrtx_in_ci.pVertexBindingDescriptions = pipeline->vert_input_bind_desc;
    vrtx_in_ci.vertexAttributeDescriptionCount = pipeline->vert_input_attrib_desc_count;
    vrtx_in_ci.pVertexAttributeDescriptions = pipeline->vert_input_attrib_desc;

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

    VkPipelineViewportStateCreateInfo vprt_st_ci = { 0 };
    vprt_st_ci.sType
        = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    vprt_st_ci.viewportCount = 1;
    vprt_st_ci.pViewports = &viewport;
    vprt_st_ci.scissorCount = 1;
    vprt_st_ci.pScissors = &scissor;

    VkPipelineRasterizationStateCreateInfo rstr_ci = { 0 };
    rstr_ci.sType 
        = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rstr_ci.depthClampEnable = VK_FALSE; // requires gpu feature 
    // being enabled
    rstr_ci.rasterizerDiscardEnable = VK_FALSE;
    rstr_ci.polygonMode = VK_POLYGON_MODE_FILL;
    // VK_POLYGON_MODE_FILL: fill the area of the polygon with fragments
    // VK_POLYGON_MODE_LINE: polygon edges are drawn as lines
    // VK_POLYGON_MODE_POINT: polygon vertices are drawn as points
    rstr_ci.lineWidth = 1.0f;
    rstr_ci.cullMode = VK_CULL_MODE_BACK_BIT; // cull back faces
    rstr_ci.frontFace = VK_FRONT_FACE_CLOCKWISE; // vertex order
    // depth bias usually used in shadow maps
    rstr_ci.depthBiasEnable = VK_FALSE;
    rstr_ci.depthBiasConstantFactor = 0.0f; 
    rstr_ci.depthBiasClamp = 0.0f; 
    rstr_ci.depthBiasSlopeFactor = 0.0f; 

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
    VkPipelineColorBlendAttachmentState cb_att_ci = { 0 };
    cb_att_ci.colorWriteMask = VK_COLOR_COMPONENT_R_BIT 
        | VK_COLOR_COMPONENT_G_BIT 
        | VK_COLOR_COMPONENT_B_BIT 
        | VK_COLOR_COMPONENT_A_BIT;
    cb_att_ci.blendEnable = VK_FALSE;
    cb_att_ci.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;
    cb_att_ci.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;
    cb_att_ci.colorBlendOp = VK_BLEND_OP_ADD;
    cb_att_ci.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
    cb_att_ci.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
    cb_att_ci.alphaBlendOp = VK_BLEND_OP_ADD;

    // color blend state
    VkPipelineColorBlendStateCreateInfo cb_st_ci = { 0 };
    cb_st_ci.sType 
        = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    cb_st_ci.logicOpEnable = VK_FALSE;
    cb_st_ci.logicOp = VK_LOGIC_OP_COPY; 
    cb_st_ci.attachmentCount = 1;
    cb_st_ci.pAttachments = &cb_att_ci;
    cb_st_ci.blendConstants[0] = 0.0f; 
    cb_st_ci.blendConstants[1] = 0.0f; 
    cb_st_ci.blendConstants[2] = 0.0f; 
    cb_st_ci.blendConstants[3] = 0.0f; 

    // create the pipeline layout
    VkPipelineLayoutCreateInfo pl_ci = { 0 };
    pl_ci.sType 
        = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pl_ci.setLayoutCount = 0; 
    pl_ci.pSetLayouts = NULL; 
    pl_ci.pushConstantRangeCount = 0; 
    pl_ci.pPushConstantRanges = NULL;

    res = vkCreatePipelineLayout(
        device, 
        &pl_ci, 
        NULL, 
        &pipeline->pipeline_layout);

    if (res != VK_SUCCESS) {
        G_Log("ERROR", "Failed to create pipeline layout.");
        return res;
    }

    VkGraphicsPipelineCreateInfo p_ci = { 0 };
    p_ci.sType 
        = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    p_ci.stageCount = 2;
    p_ci.pStages = shader_stages;
    p_ci.pVertexInputState = &vrtx_in_ci;
    p_ci.pInputAssemblyState = &input_assembly;
    p_ci.pViewportState = &vprt_st_ci;
    p_ci.pRasterizationState = &rstr_ci;
    p_ci.pMultisampleState = &multisampling;
    p_ci.pDepthStencilState = NULL; // we will do this later
    p_ci.pColorBlendState = &cb_st_ci;
    p_ci.pDynamicState = &dyn_state_create_info;
    p_ci.layout = pipeline->pipeline_layout;
    p_ci.renderPass = render_pass;
    p_ci.subpass = 0;
    // base pipelines are optional - allows you to create pipelines based on
    // preexisting pipelines
    p_ci.basePipelineHandle = VK_NULL_HANDLE;
    // p_ci.basePipelineIndex = -1;

    res = vkCreateGraphicsPipelines(
        device,
        VK_NULL_HANDLE,
        1,
        &p_ci,
        NULL,
        &pipeline->pipeline);

    if (res != VK_SUCCESS) {
        G_Log("ERROR","Failed to create graphics pipeline.");
        return res;
    }

    // free shader data
    // C_FreeFileBuffer(&vert_shader);
    // C_FreeFileBuffer(&frag_shader);

    // safe to delete shader modules now
    vkDestroyShaderModule(device, vert_shader_mod, NULL);
    vkDestroyShaderModule(device, frag_shader_mod, NULL);

    return 1;
}

VkShaderModule 
VKH_CreateShaderModule(VkDevice device, const buffer* binary) {
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
VKH_CreateRenderPass(
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

    VkRenderPassCreateInfo rp_ci = { 0 };
    rp_ci.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    rp_ci.attachmentCount = 1;
    rp_ci.pAttachments = &color_attach;
    rp_ci.subpassCount = 1;
    rp_ci.pSubpasses = &subpass;
    rp_ci.dependencyCount = 1;
    rp_ci.pDependencies = &dependency;
    

    if (vkCreateRenderPass(
        device, 
        &rp_ci, 
        NULL, 
        render_pass
    ) != VK_SUCCESS) {
        return 0;
    }

    return 1;
}

int
VKH_RecordCommandBuffer(
    VkCommandBuffer buffer,
    Uint32 image_index,
    VkRenderPass render_pass,
    VkExtent2D extent,
    VkPipeline pipeline,
    VKH_FramebufferList framebuffers,
    VkBuffer vertex_buffer
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
    render_pass_info.framebuffer = framebuffers.data[image_index];
    render_pass_info.renderArea.offset = (VkOffset2D) { 0, 0 };
    render_pass_info.renderArea.extent = extent;

    // using black for the clear color with 100% opacity
    VkClearValue clearColor = (VkClearValue) {{{0.0f, 0.0f, 0.0f, 1.0f}}};
    render_pass_info.clearValueCount = 1;
    render_pass_info.pClearValues = &clearColor;

    vkCmdBeginRenderPass(buffer, &render_pass_info, VK_SUBPASS_CONTENTS_INLINE);

        vkCmdBindPipeline(buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);

        // the viewport and scissor state for the pipeline we created was 
        // made to be dynamic, so we need to pass it to the command buffer
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

        VkBuffer vertexBuffers[] = { vertex_buffer };
        VkDeviceSize offsets[] = { 0 };
        vkCmdBindVertexBuffers(buffer, 0, 1, vertexBuffers, offsets);

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
VKH_CheckValidationLayerSupport() {
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

void
VKH_GetBindingDescription(
    Uint32* strides, 
    Uint32 size, 
    VkVertexInputBindingDescription* out) {
    // assumes out is already allocated
    // strides must be the same size

    for (Uint32 i = 0; i < size; i++) {
        out[i].binding = 0;
        out[i].stride = strides[i];
        out[i].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
    }
}

void
VKH_GetAttributeDescriptions(
    Uint32 binding,
    Uint32 num_attribs,
    Uint32* offsets,
    VkFormat* formats,
    VkVertexInputAttributeDescription* out
) {
    for (Uint32 i = 0; i < num_attribs; i++) {
        out[i].binding = binding;
        out[i].location = i;
        out[i].format = formats[i];
        out[i].offset = offsets[i];
    }
}

VkResult
VKH_CreatePhysicalDevice(
    VkInstance instance,
    VkSurfaceKHR surface,
    VkPhysicalDevice* gpu
) {
    /** Get the physical device */
    VkResult res = VK_SUCCESS;

    VkPhysicalDevice* phys_devices = VK_NULL_HANDLE;
    VkPhysicalDevice phys_device = VK_NULL_HANDLE;

    Uint32 device_count = 0;

    res = vkEnumeratePhysicalDevices(instance, &device_count, NULL);
    if (res != VK_SUCCESS) {
        G_Log("ERROR", "Failed to enumerate number of physical devices.");
        return res;
    }

    if (device_count == 0) {
        G_Log("ERROR", "No valid physical graphics device)(s) found.");
        return -1;
    }

    phys_devices = SDL_malloc(device_count * sizeof(gpu));
    if (!phys_devices) {
        G_Log("ERROR", "Error allocating memory for physical devices array.");
        return res;
    }

    res = vkEnumeratePhysicalDevices(
        instance, 
        &device_count, 
        phys_devices);
    if (res != VK_SUCCESS) {
        G_Log("ERROR", "Failed to enumerate physical devices to array.");
        goto cleanup;
    }

    // select the first device based on suitability
    for (Uint32 i = 0; i < device_count; i++) {
        if (VKH_IsPhysicalDeviceSuitable(phys_devices[i], surface)) {
            G_Log("INFO", "Found suitable physical device.");
            phys_device = phys_devices[i];
            break;
        }
    }

    // If this assertion failed, then there is no suitable graphics device.
    if (phys_device == VK_NULL_HANDLE) {
        G_Log("ERROR", "Failed to fetch suitable physical device.");
        res = -1;
        goto cleanup;
    }

    (*gpu) = phys_device;

cleanup:

    SDL_free(phys_devices);

    return res;
}

VkResult
VKH_CreateDevice(
    VkPhysicalDevice gpu,
    VKH_QueueFamilyIndices indices,
    VkDevice* device
) {
    VkResult res = VK_SUCCESS;

    /* create presentation queue (for delivering images to the display) */

    VkDeviceQueueCreateInfo* q_cis = NULL;
    // create unique list of queue families since the graphics family and the
    // presentation family may overlap.
    // TODO hardcoded for now. simple set implementation
    Uint32* unique_queue_families = NULL;
    Uint32 unique_queue_families_count = 0;

    if (indices.graphics_family == indices.present_family) {
        unique_queue_families_count = 1;
        // families are the same and one can be used
        unique_queue_families = SDL_malloc(
            unique_queue_families_count * sizeof(Uint32)
        );

        unique_queue_families[0] = indices.graphics_family;
    } else {
        unique_queue_families_count = 2;
        // families are NOT the same and both must be added
        unique_queue_families = SDL_malloc(
            unique_queue_families_count * sizeof(Uint32)
        );

        unique_queue_families[0] = indices.graphics_family;
        unique_queue_families[1] = indices.present_family;
    }

    q_cis = SDL_malloc(
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
        q_cis[i] = info;
    }

    // sets everything to false
    VkPhysicalDeviceFeatures device_feats = { 0 };

    VkDeviceCreateInfo ci = { 0 };
    ci.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    ci.pQueueCreateInfos = q_cis;
    ci.queueCreateInfoCount = unique_queue_families_count;
    ci.pEnabledFeatures = &device_feats;
    ci.enabledExtensionCount = CGAME_REQURIED_EXTENSIONS;
    ci.ppEnabledExtensionNames = required_exts;

    res = vkCreateDevice(gpu, &ci, NULL, device);

    if (res != VK_SUCCESS) {            
        G_Log("ERROR", "Error creating logical device.");
        goto cleanup;
    }

cleanup:

    SDL_free(unique_queue_families);
    SDL_free(q_cis);

    return res;
}

int
VKH_CreateSurface(
    SDL_Window* handle, 
    VkInstance instance, 
    VkSurfaceKHR* surface
) {
    if (!SDL_Vulkan_CreateSurface(
        handle, 
        instance, 
        NULL, 
        surface
    )) {
        G_Log("ERROR", "Error creating SDL3 Vulkan surface.");
        G_Log("SDL ERROR", SDL_GetError());
        return 1;
    }

    return 0;
}

VkResult
VKH_CreateSwapchain(
    SDL_Window* window_handle,
    VkPhysicalDevice gpu,
    VkDevice device,
    VkSurfaceKHR surface,
    VKH_QueueFamilyIndices indices,
    VKH_ImageList* images,
    VkSwapchainKHR* out_swapchain,
    VkSurfaceFormatKHR* out_surface_format,
    VkExtent2D* out_surface_extent
) {
    VkResult res = VK_SUCCESS;
    /* Create swap chain */
    VKH_SwapchainSupportDetails ss = VKH_QuerySwapChainSupport(
        gpu, surface
    );
    VkSurfaceFormatKHR sf = VKH_ChooseSwapSurfaceFormat(
        ss.formats,
        ss.formats_count
    );
    VkPresentModeKHR pm = VKH_ChooseSwapPresentMode(
        ss.present_modes,
        ss.present_modes_count
    );
    VkExtent2D extent = VKH_ChooseSwapExtent(
        window_handle,
        &ss.capabilities
    );

    *out_surface_format = sf;
    *out_surface_extent = extent;

    // get the minimum number of images to keep in the swap chain

    Uint32 image_count = ss.capabilities.minImageCount + 1;
    if (
        ss.capabilities.maxImageCount > 0
        && image_count > ss.capabilities.maxImageCount
    ) {
        image_count = ss.capabilities.maxImageCount;
    }

    VkSwapchainCreateInfoKHR ci = { 0 };
    ci.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    ci.surface = surface;
    ci.minImageCount = image_count;
    ci.imageFormat = sf.format;
    ci.imageColorSpace = sf.colorSpace;
    ci.imageExtent = extent;
    ci.imageArrayLayers = 1; // always 1 unless creating a
    // stereoscopic 3D application
    ci.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    // use VK_IMAGE_USAGE_TRANSFER_DST_BIT for post processing, but the image 
    // will be used as a simple color sequence for displaying.
    
    // configure the swapchain create info based on the current queue families
    Uint32 swapchain_QueueFamilyIndices[2] = {
        indices.graphics_family,
        indices.present_family
    };

    if (indices.graphics_family != indices.present_family) {
        // graphics family and present family are not the same - 2 queue 
        // families
        // VK_SHARING_MODE_CONCURRENT: Images can be used across multiple queue 
        // families without explicit ownership transfers.
        ci.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        ci.queueFamilyIndexCount = 2;
        ci.pQueueFamilyIndices = swapchain_QueueFamilyIndices;
    } else {
        // graphics family and present family are the same
        ci.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        // VK_SHARING_MODE_EXCLUSIVE An image is owned by one queue family at a 
        // time and ownership must be explicitly transferred before using it in 
        // another queue family. This option offers the best performance.
        ci.queueFamilyIndexCount = 0; // optional
        ci.pQueueFamilyIndices = NULL; // optional
    }

    // transform the image in the swap chain
    ci.preTransform = 
        ss.capabilities.currentTransform;
    
    // disable alpha blending with other windows in the window system
    ci.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    ci.presentMode = pm;
    ci.clipped = VK_TRUE; // click pixels outside the extent
    // when the window gets resized or changed, the swapchain needs to be 
    // recreated.
    ci.oldSwapchain = VK_NULL_HANDLE;

    res = vkCreateSwapchainKHR(
        device, 
        &ci, 
        NULL, 
        out_swapchain
    );
    if (res != VK_SUCCESS) {
        G_Log("ERROR", "Error creating swapchain.");
        goto cleanup;
    }

    /* create swapchain images */

    res = vkGetSwapchainImagesKHR(
        device, 
        *out_swapchain, 
        &images->size, 
        NULL
    );

    if (res != VK_SUCCESS) {
        G_Log("ERROR", "Could not enumerate swapchain image size.");
        return res;
    }

    images->data = SDL_malloc(images->size * sizeof(*images->data));

    if (!images->data) {
        G_Log("ERROR", "Failed to allocate memory for swapchain images.");
        return res;
    }

    res = vkGetSwapchainImagesKHR(
        device, 
        *out_swapchain, 
        &images->size, 
        images->data
    );
    if (res != VK_SUCCESS) {
        G_Log("ERROR", "Could not fetch swapchain images.");
        SDL_free(images->data);
        return res;
    }

cleanup: 

    return res;
}

VkResult
VKH_CreateImageViews(
    VkDevice device,
    VkFormat format,
    const VKH_ImageList* images,
    VKH_ImageViewList* image_views
) {
    VkResult res = VK_SUCCESS;
    image_views->data = SDL_malloc(image_views->size * sizeof(
        image_views->data[0]
    ));

    for (Uint32 i = 0; i < image_views->size; i++) {
        VkImageViewCreateInfo ci = { 0 };
        ci.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        ci.image = images->data[i];
        // interpret images as 2d
        ci.viewType = VK_IMAGE_VIEW_TYPE_2D;
        // swapchain format determines color format
        ci.format = format;
        // do not change the color channels in the image view
        ci.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
        ci.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
        ci.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
        ci.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
        ci.subresourceRange.aspectMask 
            = VK_IMAGE_ASPECT_COLOR_BIT;
        ci.subresourceRange.baseMipLevel = 0;
        ci.subresourceRange.levelCount = 1;
        ci.subresourceRange.baseArrayLayer = 0;
        ci.subresourceRange.layerCount = 1;

        // create image view
        res = vkCreateImageView(
            device, 
            &ci, 
            NULL, 
            &image_views->data[i]
        );
        if (res != VK_SUCCESS) {
            G_Log("ERROR", "Failed to create image views.");
            SDL_free(image_views->data);
            return res;
        }
    }

    return res;
}

VkResult
VKH_CreateFramebuffers(
    VkDevice device,
    VkRenderPass render_pass,
    VkExtent2D extent,
    VKH_ImageViewList image_views,
    VKH_FramebufferList* framebuffers
) {
    VkResult res = VK_SUCCESS;
    framebuffers->data = SDL_malloc(framebuffers->size * sizeof(
        framebuffers->data[0]
    ));
    if (!framebuffers->data) {
        G_Log("ERROR", "Error allocating memory for framebuffers.");
        return -1;
    }
    for (Uint32 i = 0; i < framebuffers->size; i++) {
        // array of image views
        VkImageView attachments[] = {
            image_views.data[i]
        };

        VkFramebufferCreateInfo ci = { 0 };
        ci.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        ci.renderPass = render_pass;
        ci.attachmentCount = 1;
        ci.pAttachments = attachments;
        ci.width = extent.width;
        ci.height = extent.height;
        ci.layers = 1;

        res = vkCreateFramebuffer(
            device,
            &ci,
            NULL,
            &framebuffers->data[i]
        );

        if (res != VK_SUCCESS) {
            G_Log("ERROR", "Error while creating framebuffers.");
            SDL_free(framebuffers->data);
            return res;
        }
    }

    return res;
}

VkResult
VKH_CreateCommandPool(
    VkDevice device,
    VKH_QueueFamilyIndices indices,
    VkCommandPool* pool
) {
    VkResult res = VK_SUCCESS;
    VkCommandPoolCreateInfo pool_ci = { 0 };
    pool_ci.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    pool_ci.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    pool_ci.queueFamilyIndex = indices.graphics_family;

    res = vkCreateCommandPool(
        device,
        &pool_ci,
        NULL,
        pool);

    if (res != VK_SUCCESS) {
        G_Log("ERROR", "Error creating command pool.");
    }

    return res;
}

VkResult
VKH_CreateCommandBuffer(
    VkDevice device,
    VkCommandPool pool,
    VKH_CommandBufferList* command_buffers
) {
    VkResult res = VK_SUCCESS;
    VkCommandBufferAllocateInfo cb_ai = { 0 };
    cb_ai.sType 
        = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    cb_ai.commandPool = pool;
    cb_ai.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    cb_ai.commandBufferCount = command_buffers->size;

    res = vkAllocateCommandBuffers(
        device,
        &cb_ai,
        command_buffers->data
    );

    if (res != VK_SUCCESS) {
        G_Log("ERROR", "Error creating command buffer.");
    }

    return res;
}

VkResult
VKH_CreateSemaphore(
    VkDevice device,
    VkSemaphore* semaphore
) {
    VkResult res = VK_SUCCESS;
    VkSemaphoreCreateInfo semaphore_info = { 0 };
    semaphore_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    res = vkCreateSemaphore(
        device, 
        &semaphore_info, 
        NULL, 
        semaphore);

    if (res != VK_SUCCESS) {
        G_Log("ERROR", "Error creating image available semaphore.");
    }

    return res;
}

VkResult
VKH_CreateFence(
    VkDevice device,
    VkFence* fence
) {
    VkResult res = VK_SUCCESS;
    VkFenceCreateInfo fence_create_info = { 0 };
    fence_create_info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fence_create_info.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    res = vkCreateFence(
        device, 
        &fence_create_info, 
        NULL, 
        fence);

    if (res != VK_SUCCESS) {
        G_Log("ERROR", "Failed to create fence object.");
    }

    return res;
}

int
VKH_FindMemoryType(
    VkPhysicalDevice gpu,
    Uint32 type_filter,
    VkMemoryPropertyFlags properties,
    Uint32* flags
) {
    VkPhysicalDeviceMemoryProperties gpu_properties;
    vkGetPhysicalDeviceMemoryProperties(gpu, &gpu_properties);

    for (Uint32 i = 0; i < gpu_properties.memoryTypeCount; i++) {
        Uint32 filtered = type_filter & (1 << i);
        Uint32 props_match 
            = (gpu_properties.memoryTypes[i].propertyFlags & properties) == properties;
        if (filtered && props_match) {
            (*flags) = i;
            return 1;
        }
    }

    return 0;
}

VkResult
VKH_CreateVertexBuffer(
    VkDevice device, 
    VkPhysicalDevice gpu,
    const Vertex* vertices, 
    Uint32 size, 
    VkBuffer* buffer,
    VkDeviceMemory* memory
) {
    VkResult res = VK_SUCCESS;
    VkBufferCreateInfo buffer_info = { 0 };
    buffer_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    buffer_info.size = sizeof(vertices[0]) * size;
    buffer_info.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
    buffer_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    res = vkCreateBuffer(
        device, 
        &buffer_info, 
        NULL, 
        buffer);
        
    if (res != VK_SUCCESS) {
        G_Log("ERROR", "Failed to create vertex buffer.");
    }

    VkMemoryRequirements memRequirements = { 0 };
    vkGetBufferMemoryRequirements(device, *buffer, &memRequirements);

    VkMemoryAllocateInfo alloc_info = { 0 };
    alloc_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    alloc_info.allocationSize = memRequirements.size;

    // set memoryTypeIndex for allocation info
    if (!VKH_FindMemoryType(
        gpu,
        memRequirements.memoryTypeBits, 
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT 
            | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
        &alloc_info.memoryTypeIndex)
    ) {
        G_Log("ERROR", "Failed to find suitable memory type to create vertex \
            buffer.");
        return VK_ERROR_UNKNOWN;
    }

    res = vkAllocateMemory(device, &alloc_info, NULL, memory);
    if (res != VK_SUCCESS) {
        G_Log("ERROR", "Failed to allocate vertex buffer memory.");
    }

    vkBindBufferMemory(device, *buffer, *memory, 0);

    /* Map memory */
    void* data;
    vkMapMemory(device, *memory, 0, buffer_info.size, 0, &data);
        memcpy(data, vertices, (size_t) buffer_info.size);
    vkUnmapMemory(device, *memory);
    /* Unmap memory */

    return res;
}