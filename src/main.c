
#include "game/engine.h"

int main(int argc, char** argv) {
    printf("Hello world");

    Engine engine;

    engine_init(&engine);
    engine_run(&engine);
    engine_stop(&engine);

    return 0;
}