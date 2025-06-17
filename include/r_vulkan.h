#ifndef VULKAN_CGAME_H_
#define VULKAN_CGAME_H_

#include <vulkan/vulkan.h>

#include "SDL3/SDL.h"

#include "g_window.h"
#include "c_utils.h"

#define CGAME_REQURIED_EXTENSIONS 1
#define CGAME_DYNAMIC_STATES_COUNT 2
#define CGAME_VALIDATION_LAYERS_COUNT 1

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

    /* Image stuff */

    VkImage* images;
    Uint32 image_size;
    VkImageView* image_views;

    VkCommandPool pool;

    #ifdef NDEBUG // Debugging properties

    VkDebugUtilsMessengerEXT debug_messenger;

    #endif

} VKH_VulkanInstance;

typedef struct {
    VkShaderModule vert_shader;
    VkShaderModule frag_shader;

    VkDescriptorSetLayout descriptor_layout;
    VkPipelineLayout pipeline_layout;
    VkPipeline pipeline;
} VKH_GraphicsPipeline;

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
 * @param window The size of the window.
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
 * @param rendeVKH_pass The render pass object.
 * @param out_layout Output parameter - The new pipeline layout.
 * @param pipeline Output parameter - the new pipeline.
 * @returns True/false
 */
int
VKH_CreateGraphicsPipeline(
    VkDevice device, 
    VkExtent2D extent, 
    VkRenderPass rendeVKH_pass,
    VkPipelineLayout* out_layout,
    VkPipeline* pipeline);

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
    VkRenderPass* rendeVKH_pass);

/**
 * Create the framebuffers for this applicaton.
 * @param device
 * @param framebuffers
 * @param size
 * @returns True/false
 */
int
VKH_CreateFramebuffers(
    VkDevice device,
    VkFramebuffer** framebuffers, 
    Uint32* size);

/**
 * Record to a command buffer.
 * @param buffer
 * @param image_index Index of the current swap chain image.
 * @param rendeVKH_pass
 * @param extent
 * @returns True/false
 */
int
VKH_RecordCommandBuffer(
    VkCommandBuffer buffer,
    VkFramebuffer* framebuffers,
    Uint32 image_index,
    VkRenderPass rendeVKH_pass,
    VkExtent2D extent,
    VkPipeline pipeline
);

/**
 * Draws a frame.
 * @param device
 * @param fence
 * @param swapchain
 * @param image_available
 * @param buffer
 * @param pipeline
 * @returns true/false
 */
int
VKH_DrawFrame(
    VkDevice device,
    VkFence fence,
    VkSwapchainKHR swapchain,
    VkSemaphore image_available,
    VkSemaphore rendeVKH_finished,
    VkCommandBuffer buffer,
    VkQueue graphics_queue,
    VkFramebuffer* framebuffers,
    VkRenderPass rendeVKH_pass,
    VkExtent2D extent,
    VkQueue present_queue,
    VkPipeline pipeline
);

/**
 * Determine vulkan's validation layer support based on our validation_layers
 * variable.
 * @returns True if vulkan supports all of our specified validation_layers, 
 * false if even one is not.
 */
int
VKH_CheckValidationLayerSupport();

#endif // VULKAN_CGAME_H_