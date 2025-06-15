#ifndef UTILS_H_
#define UTILS_H_

#include <SDL3/SDL.h>

typedef struct {
    Uint64 size;
    char* data;
} buffer;

/**
 * Reads a file and returns a heap allocated buffer of bytes.
 * @param filename The filename.
 * @param The contents of the file and its size.
 * @returns Success or failure.
 */
int
C_ReadBinaryFile(const char* filename, buffer* buf);

/**
 * Frees the file buffer data.
 * @params buf The buffer containing the file contents.
 */
void
C_FreeFileBuffer(buffer* buf);

#endif