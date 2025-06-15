#include "c_utils.h"
#include "c_log.h"

#include <stdio.h>

int
C_ReadBinaryFile(const char* filename, buffer* buf) {
    FILE* shaderfile = fopen(filename, "rb");

    if (!shaderfile) {
        G_Log("ERROR", "Failed to open file.");
        return 0;
    }

    // get size

    fseek(shaderfile, 0, SEEK_END);
    Uint64 size = ftell(shaderfile);
    fseek(shaderfile, 0, SEEK_SET);

    // allocate buffer. no need to null terminate as its binary data.

    char* buffer = SDL_malloc(size);
    if (!buffer) {
        G_Log("ERROR", "Failed to allocate memory for file.");
        fclose(shaderfile);
        return 0;
    }

    // read file

    Uint64 read = fread(buffer, 1, size, shaderfile);
    if (read != size) {
        G_Log("ERROR", "Inconsistent file and buffer size for file.");
        SDL_free(buffer);
        fclose(shaderfile);
        return 0;
    }
    fclose(shaderfile);

    buf->data = buffer;
    buf->size = read;

    return 1;
}

void
C_FreeFileBuffer(buffer* buf) {
    if (buf->data) {
        SDL_free(buf->data);
        buf->data = NULL;
        buf->size = 0;
    }
}