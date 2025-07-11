#ifndef VULKAN_CGAME_H_
#define VULKAN_CGAME_H_

#include <vulkan/vulkan.h>
#include <stddef.h>
#include <SDL3/SDL.h>

#include "g_window.h"
#include "c_utils.h"
#include "r_vertex.h"

#define CGAME_REQURIED_EXTENSIONS 1
#define CGAME_DYNAMIC_STATES_COUNT 2
#define CGAME_VALIDATION_LAYERS_COUNT 1

#define CGAME_MAX_SEMAPHORES 32
#define CGAME_MAX_FENCES 32

/* Determines if we should use validation layers */
extern const int enable_validation_layers;

/* List of required extensions */
extern const char* required_exts[CGAME_REQURIED_EXTENSIONS];

/* List of dynamic states used by the application */
extern const VkDynamicState dynamic_states[CGAME_DYNAMIC_STATES_COUNT];

/* List of validation layers */
extern const char* validation_layers[CGAME_VALIDATION_LAYERS_COUNT];

typedef struct {

    int none_graphics_family;
    int none_present_family;
    Uint32 graphics_family;
    Uint32 present_family;

} VKH_QueueFamilyIndices;

typedef struct {

    VkSurfaceCapabilitiesKHR capabilities;

    VkSurfaceFormatKHR* formats;
    Uint32 formats_count;

    VkPresentModeKHR* present_modes;
    Uint32 present_modes_count;

} VKH_SwapchainSupportDetails;

typedef struct {

    VkShaderModule vert_shader;
    VkShaderModule frag_shader;

    VkVertexInputBindingDescription* vert_input_bind_desc;
    Uint32 vert_input_bind_desc_count;
    VkVertexInputAttributeDescription* vert_input_attrib_desc;
    Uint32 vert_input_attrib_desc_count;

    VkDescriptorSetLayout descriptor_layout;
    VkPipelineLayout pipeline_layout;
    VkPipeline pipeline;

} VKH_GraphicsPipeline;

/**
 * Specifies a list of semaphores.
 */
typedef struct {

    /* List of semaphores. */
    VkSemaphore data[CGAME_MAX_SEMAPHORES];

    /* Number of elements. */
    Uint32 size;

} VKH_SemaphoreList;

/**
 * Specifies a list of fences.
 */
typedef struct {

    /* List of fences. */
    VkFence data[CGAME_MAX_FENCES];

    /* Number of fences. */
    Uint32 size;

} VKH_FenceList;

/**
 * Specifies a list of frame buffers.
 */
typedef struct {

    /* Framebuffer list. */
    VkFramebuffer* data;

    /* Number of framebuffers being used. */
    Uint32 size;

} VKH_FramebufferList;

/**
 * Specifies a list of image views.
 */
typedef struct {

    /* Image view list. */
    VkImageView* data;

    /* Number of image views being used. */
    Uint32 size;

} VKH_ImageViewList;

/**
 * Specifies a list of images.
 */
typedef struct {
     
    /* Image list. */
    VkImage* data;

    /* Number of image views being used. */
    Uint32 size;

} VKH_ImageList;

/**
 * Specifies a list of command buffers.
 */
typedef struct {

    /* Command buffer list. */
    VkCommandBuffer* data;

    /* Number of command buffers in the list. */
    Uint32 size;

} VKH_CommandBufferList;


typedef struct VKH_VulkanState {
    Window* window;

    /* High level vulkan components */

    VkInstance instance;
    VkPhysicalDevice gpu;
    VkDevice device;
    VKH_QueueFamilyIndices queue_families;

    /* presentation */

    VkQueue graphics_queue;
    VkQueue present_queue;
    VkQueue compute_queue;
    VkSurfaceKHR surface;
    VkSwapchainKHR swapchain;
    VkSurfaceFormatKHR swapchain_format;
    VkExtent2D swapchain_extent;

    /* vertex buffer */
    VkBuffer vertex_buffer;
    VkDeviceMemory vertex_buffer_memory;

    /* Image stuff */

    VKH_ImageList images;
    VKH_ImageViewList image_views;

    VkCommandPool pool;

    VkRenderPass render_pass;

    VKH_GraphicsPipeline pipeline;
    VKH_CommandBufferList command_buffers;
    VKH_FramebufferList framebuffers;
    VKH_SemaphoreList image_available;
    VKH_SemaphoreList render_finished;
    VKH_FenceList inflight_fence;

    Uint32 framebuffer_resized;

    #ifdef NDEBUG // Debugging properties

    VkDebugUtilsMessengerEXT debug_messenger;

    #endif

} VKH_VulkanState;

/**
 * Creates a Vulkan instance.
 * 
 * @param vk The vulkan instance to instantiate.
 * @returns True or false
 */
VkResult
VKH_CreateInstance(VkInstance* vk);

/**
 * Determine if the provided physical device is suitable
 * for rendering.
 * 
 * Must be a discrete GPU and provide geometry shaders.
 * @param device The physical graphics device.
 * @returns True or false
 */
int
VKH_IsPhysicalDeviceSuitable(VkPhysicalDevice device, VkSurfaceKHR surface);

/**
 * Finds the queue families used by the provided graphics device.
 * @param device The graphics device.
 * @returns Flags detailing the used queue families.
 */
VKH_QueueFamilyIndices
VKH_FindQueueFamilies(VkPhysicalDevice device, VkSurfaceKHR surface);

/**
 * Get the supported details about the swap chain.
 * @param device The physical graphics device.
 * @param surface The surface to draw on.
 * @returns The swapchain support details, including the capabilities, drawing
 * extent, color space, present modes, formats, and some more.
 */
VKH_SwapchainSupportDetails
VKH_QuerySwapChainSupport(VkPhysicalDevice device, VkSurfaceKHR surface);

/**
 * Choose the best surface format for the swap chain for the purposes of this
 * application.
 * @param formats The list of surface formats supported by the swap chain.
 * @param size The number of surface formats.
 * @param The best surface format, falling back on the first provided one.
 */
VkSurfaceFormatKHR
VKH_ChooseSwapSurfaceFormat(const VkSurfaceFormatKHR* formats, Uint32 size);

/**
 * Chooses the best present mode for the swap chain given the list of present
 * modes supported by the swap chain.
 * @param present_modes The list of present modes.
 * @param size The number of present modes.
 * @returns The best present mode for the application, falling back on the FIFO
 * present mode.
 */
VkPresentModeKHR
VKH_ChooseSwapPresentMode(const VkPresentModeKHR* present_modes, Uint32 size);

/**
 * Chooses the extent of the swap chain based on our capabilities and the window
 * size.
 * @param window The window.
 * @param capabilities The capabilities of the surface.
 * @return The minimum and maximum extents of the Vulkan surface.
 */
VkExtent2D 
VKH_ChooseSwapExtent(
    SDL_Window* window, 
    const VkSurfaceCapabilitiesKHR* capabilities);

/**
 * Create the graphics pipeline.
 * @param device The logical device.
 * @param extent The extent of the Vulkan surface.
 * @param render_pass The render pass object.
 * @param pipeline Output parameter - the new pipeline.
 * @returns True/false
 */
VkResult
VKH_CreateGraphicsPipeline(
    VkDevice device, 
    VkExtent2D extent, 
    VkRenderPass render_pass,
    VKH_GraphicsPipeline* pipeline);

/**
 * Creates the shader module.
 * @param binary Contains the SPIV-R binary data.
 * @returns VkShaderModule
 */
VkShaderModule
VKH_CreateShaderModule(VkDevice device, const buffer* binary);

/**
 * Creates the render pass.
 * @param device The logical device.
 * @param image_format The image format.
 * @param rendeVKH_pass Output variable - the render pass created in the process.
 * @returns True/false
 */
int
VKH_CreateRenderPass(
    VkDevice device, 
    VkFormat image_format, 
    VkRenderPass* render_pass);

/**
 * Record to a command buffer.
 * @param buffer
 * @param image_index
 * @param render_pass
 * @param extent
 * @param pipeline
 * @param framebuffers
 * @returns True/false
 */
int
VKH_RecordCommandBuffer(
    VkCommandBuffer buffer,
    Uint32 image_index,
    VkRenderPass render_pass,
    VkExtent2D extent,
    VkPipeline pipeline,
    VKH_FramebufferList framebuffers,
    VkBuffer vertex_buffer);

/**
 * Determine vulkan's validation layer support based on our validation_layers
 * variable.
 * @returns True if vulkan supports all of our specified validation_layers, 
 * false if even one is not.
 */
int
VKH_CheckValidationLayerSupport();

/**
 * Gets the binding description of a vertex input.
 * 
 * @param strides The strides for each binding description.
 * @param size The number of binding descriptions to create, as well as the
 * number of strides.
 * @param out
 */
void
VKH_GetBindingDescription(Uint32* strides, Uint32 size, VkVertexInputBindingDescription* out);

/**
 * Get the attribute descriptions based on offset sizes.
 */
void
VKH_GetAttributeDescriptions(
    Uint32 binding,
    Uint32 num_attribs,
    Uint32* offsets,
    VkFormat* formats,
    VkVertexInputAttributeDescription* out
);

/**
 * Create the queue family indices for the current device.
 * 
 * @returns 
 */
int
VKH_CreateQueue(
    VkDevice device,
    VkSurfaceKHR surface,
    VKH_QueueFamilyIndices indices,
    VkQueue* queue
);

/**
 * Create a logical device.
 * 
 * @param gpu The physical device.
 * @param indices The queue family indices.
 * @param device The logical device location to create it at.
 * 
 * @returns VkResult
 */
VkResult
VKH_CreateDevice(
    VkPhysicalDevice gpu,
    VKH_QueueFamilyIndices indices,
    VkDevice* device
);

/**
 * Create the physical device.
 * 
 * @param gpu The physical device to output to.
 * @return True or false, if successful
 */
VkResult
VKH_CreatePhysicalDevice(
    VkInstance instance,
    VkSurfaceKHR surface,
    VkPhysicalDevice* gpu
);

/**
 * Create a Vulkan KHR surface object.
 * 
 * @param surface The surface object to output to.
 * @returns True or false, if passed
 */
int
VKH_CreateSurface(
    SDL_Window* handle, 
    VkInstance instance, 
    VkSurfaceKHR* surface
);

/**
 * Create the Vulkan swapchain.
 * 
 * @param windlw_handle The handle to the SDL window.
 * @param gpu The physical device.
 * @param device The logical device.
 * @param surface The KHR surface.
 * @param indices The supported queue families.
 * @param out_min_images The output for the minium number of images to store in
 * the swapchain.
 * @param out_swapchain The swapchain output.
 * @param out_surface_format The surface_format output.
 * @param out_surface_extent The extent output.
 * @returns VkResult
 */
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
);

/**
 * Create the image views used in the presentation queue.
 * 
 * @param device
 * @param image_view_count
 * @param image_views
 */
VkResult
VKH_CreateImageViews(
    VkDevice device,
    VkFormat format,
    const VKH_ImageList* images,
    VKH_ImageViewList* image_views
);

/**
 * Create the framebuffers for the application.
 * 
 * @param device
 * @param render_pass
 * @param extent
 * @param image_views
 * @param framebuffers
 */
VkResult
VKH_CreateFramebuffers(
    VkDevice device,
    VkRenderPass render_pass,
    VkExtent2D extent,
    VKH_ImageViewList image_views,
    VKH_FramebufferList* framebuffers
);

/**
 * Creates the command pool for the graphics queue family.
 * 
 * @param device
 * @param indices
 * @param pool
 */
VkResult
VKH_CreateCommandPool(
    VkDevice device,
    VKH_QueueFamilyIndices indices,
    VkCommandPool* pool
);

/**
 * Create the command buffer used to store draw commands.
 * 
 * @param device
 * @param pool
 * @param buffer
 */
VkResult
VKH_CreateCommandBuffer(
    VkDevice device,
    VkCommandPool pool,
    VKH_CommandBufferList* command_buffers
);

/**
 * Creates a semaphore thats used between threaded calls to the gpu.
 * 
 * @param device
 * @param semaphore
 */
VkResult
VKH_CreateSemaphore(
    VkDevice device,
    VkSemaphore* semaphore
);

/**
 * Creates a fence for mutlithreaded purposes between the host and the gpu.
 * 
 * @param device
 * @param fence
 */
VkResult
VKH_CreateFence(
    VkDevice device,
    VkFence* fence
);

/**
 * Finds the memory type of the physical device.
 */
int
VKH_FindMemoryType(
    VkPhysicalDevice gpu,
    Uint32 type_filter,
    VkMemoryPropertyFlags properties,
    Uint32* out
);

/**
 * Creates a vertex buffer from the specified vertex array.
 * 
 * @param device
 * @param vertices
 * @param size
 * @param buffer
 * @param memory
 * 
 * @return `VkResult`
 */
VkResult
VKH_CreateVertexBuffer(
    VkDevice device, 
    VkPhysicalDevice gpu,
    const Vertex* vertices, 
    Uint32 size, 
    VkBuffer* buffer,
    VkDeviceMemory* memory);

#endif // VULKAN_CGAME_H_