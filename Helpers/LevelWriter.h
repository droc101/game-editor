//
// Created by droc101 on 1/17/25.
//

#ifndef LEVELWRITER_H
#define LEVELWRITER_H

#include "../defines.h"

void WriteLevel(const Level *level, const char *path);

Level *ReadLevel(const char *path);

#endif //LEVELWRITER_H
