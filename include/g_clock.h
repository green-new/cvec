#ifndef CLOCK_H_
#define CLOCK_H_

#include "SDL3/SDL.h"

/**
 * @struct clock
 * @brief Represents the game clock.
 */
typedef struct clock {
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
    double renderFreq;

    /* The physics frequency. */
    double physFreq;

    /* The FPS frequency. */
    double fpsFreq;
} clock;

/**
 * @brief Update the provided clock object.
 * @param clock The clock object.
 */
void
clock_update(clock* clock) {
    clock->prevTime = clock->currTime;
    clock->currTime = SDL_GetTicks();
    clock->deltaTime = clock->currTime - clock->prevTime;

    clock->physTick = ((float)(clock->currTime - clock->physTime) > (1.0f / clock->physFreq));
    clock->renderTick = ((float)(clock->currTime - clock->renderTime) > (1.0f / clock->renderFreq));
    clock->fpsTick = ((float)(clock->currTime - clock->fpsTime) > (1.0f / 1000.0f));

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
clock_reset(clock* clock) {
    // reset all values in the clock
    clock->currTime = 0.0;
    clock->prevTime = 0.0;
    clock->deltaTime = 0.0;
    clock->physTick = 0;
    clock->renderTick = 0;
    clock->fpsTick = 0;
    clock->physTime = 0.0;
    clock->renderTime = 0.0;
    clock->fpsTime = 0.0;
    clock->physDelta = 0.0;
    clock->renderDelta = 0.0;
    clock->fpsDelta = 0.0;
}


#endif