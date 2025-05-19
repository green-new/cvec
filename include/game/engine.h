#ifndef ENGINE_H__
#define ENGINE_H__

typedef enum EngineResult {
    SUCCESS = 0,
    FAILURE = 1
} EngineResult;

typedef struct Engine {

    

} Engine;

EngineResult
engine_init(Engine* engine);

void
engine_start(Engine* engine);

void
engine_stop(Engine* engine);


#endif