//
// Created by droc101 on 1/13/25.
//

#include "FileIO.h"

#include <stdio.h>
#include <stdlib.h>

char *ReadFile(const char *path)
{
    FILE *f = fopen(path, "r");
    if (!f)
    {
        return NULL;
    }
    fseek(f, 0, SEEK_END);
    const size_t length = ftell(f);
    char *buffer = malloc(length + 1);
    fseek(f, 0, SEEK_SET);
    fread(buffer, 1, length, f);
    buffer[length] = '\0';
    fclose(f);

    return buffer;
}
