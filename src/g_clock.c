#include "g_clock.h"

void
G_ClockUpdate(Clock* clock) {
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

void
G_ClockReset(Clock* clock) {
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