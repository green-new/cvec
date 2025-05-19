#ifndef CLOCK_H__
#define CLOCK_H__


typedef struct Clock {
    double currTime;
    double prevTime;
    double deltaTime;

    double physTime;
    double renderTime;
    double fpsTime;

    int physTick;
    int renderTick;
    int fpsTick;

    const double renderFreq;
    const double physFreq;
    const double fpsFreq;
} Clock;

/**
 * Update the provided clock object.
 */
void
clock_update(Clock* clock);

/**
 * Reset the clock object.
 */
void
clock_reset(Clock* clock);


#endif