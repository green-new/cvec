/**
 * File: VKH_render.h
 * Description: Contains code definitions for the rendering part of the game.
 * Author: green
 */
#ifndef RENDER_H_
#define RENDER_H_

#include <stdio.h>
#include <stdlib.h>
#include <vulkan/vulkan.h>
#include <SDL3/SDL.h>

#include "c_utils.h"
#include "g_window.h"
#include "c_math.h"
#include "r_vulkan.h"
#include "g_clock.h"

#define NDEBUG 1 // are we debug mode?

typedef enum {
    R_SUCCESS,
    R_FAILURE,
    R_MEMORY_REFUSED,
    R_VULKAN_ERROR
} R_RenderExitCode; 

/* Contains our current render state. */
typedef struct R_RenderState {

    int initialized;

    const Window* window;

    VKH_VulkanState vk;

    Uint32 current_frame;

} R_RenderState;

/**
 * Draws to the screen.
 * 
 * @param state
 * @returns code
 */
int
R_Draw(R_RenderState* state, const Clock* clockState);

/**
 * Create the object used to house rendering properties.
 * 
 * @param state The render state to initialize.
 */
int
R_CreateRenderState(R_RenderState* state);

/**
 * Destroy the render state and all rendering objects.
 * 
 * @param state The state.
 */
int
R_DestroyRenderState(R_RenderState* state);

#endif // RENDER_H_