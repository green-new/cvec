#ifndef CLOCK_H_
#define CLOCK_H_

#include "SDL3/SDL.h"

/**
 * @struct clock
 * @brief Represents the game clock.
 */
typedef struct {
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
} Clock;

/**
 * @brief Update the provided clock object.
 * @param clock The clock object.
 */
void
clock_update(Clock* clock);

/**
 * Reset the clock object.
 */
void
clock_reset(Clock* clock);


#endif