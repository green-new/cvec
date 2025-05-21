#ifndef CLOCK_H__
#define CLOCK_H__

#include "SDL3/SDL.h"

/**
 * @brief Represents the game clock.
 */
typedef struct Clock {
    /* The current time. */
    double currTime;

    /* The previous time. */
    double prevTime;

    /* The difference between the current time and the previous time. */
    double deltaTime;

    /* The last time the physics tick was executed. */
    double physTime;

    /* The last time the render tick was executed. */
    double renderTime;

    /* The last time the FPS tick was executed. */
    double fpsTime;

    /* The difference between the current time and the last physics tick time. */
    double physDelta;

    /* The difference between the current time and the last render tick time. */
    double renderDelta;

    /* The difference between the current time and the last FPS tick time. */
    double fpsDelta;

    /* If the current tick is a physics tick or not. */
    int physTick;

    /* If the current tick is a render tick or not. */
    int renderTick;

    /* If the current tick is a FPS tick or not. */
    int fpsTick;

    /* The render frequency. */
    const double renderFreq;

    /* The physics frequency. */
    const double physFreq;

    /* The FPS frequency. */
    const double fpsFreq;
} Clock;

/**
 * @brief Update the provided clock object.
 * @param clock The clock object.
 */
void
clock_update(Clock* clock) {
    clock->prevTime = clock->currTime;
    clock->currTime = SDL_GetTicks();
    clock->deltaTime = clock->currTime - clock->prevTime;

    clock->physTick = ((float)(clock->currTime - clock->physTime) > (1.0f / clock->physFreq));
    clock->renderTick = ((float)(clock->currTime - clock->renderTime) > (1.0f / clock->renderFreq));
    clock->fpsTick = ((float)(clock->currTime - clock->renderTime) > (1.0f / 1000.0f));

    // update physics tick
    if (clock->physTick) {
        clock->physDelta = clock->currTime - clock->physTime;
        clock->physTime = clock->currTime;
    }
    
    // update render tick
    if (clock->renderTick) {
        clock->renderDelta = clock->currTime - clock->renderTime;
        clock->renderTime = clock->currTime;
    }
    
    // update fps tick
    if (clock->fpsTick) {
        clock->fpsDelta = clock->currTime - clock->fpsTime;
        clock->fpsTime = clock->currTime;
    }
}

/**
 * Reset the clock object.
 */
void
clock_reset(Clock* clock) {
    // reset all values in the clock
    clock->currTime = 0.0f;
    clock->prevTime = 0.0f;
    clock->deltaTime = 0.0f;
    clock->physTick = 0.0f;
    clock->renderTick = 0.0f;
    clock->fpsTick = 0.0f;
    clock->physTime = 0.0f;
    clock->renderTime = 0.0f;
    clock->fpsTime = 0.0f;
    clock->physDelta = 0.0f;
    clock->renderDelta = 0.0f;
    clock->fpsDelta = 0.0f;
}


#endif