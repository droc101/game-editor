//
// Created by droc101 on 1/17/25.
//

#ifndef LEVELWRITER_H
#define LEVELWRITER_H

#include "../defines.h"

/**
 * Write a level to a file
 * @param level The level
 * @param path The file path
 */
void WriteLevel(const Level *level, const char *path);

/**
 * Read a level from a file
 * @param path The file path to read from
 * @return The level read, or @c NULL on error
 */
Level *ReadLevel(const char *path);

#endif //LEVELWRITER_H
