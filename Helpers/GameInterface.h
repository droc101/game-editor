//
// Created by droc101 on 1/24/25.
//

#ifndef GAMEINTERFACE_H
#define GAMEINTERFACE_H

#include "../defines.h"

typedef char* (*GetActorNameFunc)(int actor);
typedef char* (*GetActorParamNameFunc)(int actor, byte param);
typedef int (*GetActorTypeCountFunc)();

extern GetActorNameFunc GetActorName;
extern GetActorParamNameFunc GetActorParamName;
extern GetActorTypeCountFunc GetActorTypeCount;

/**
 * Load libactor.so and get the function pointers
 * @return Whether the executable was loaded successfully
 * @todo From what I've experienced, this WILL segfault on failure
 */
bool LoadExecutable();

/**
 * Unload the executable and reset the function pointers
 */
void UnloadExecutable();

#endif //GAMEINTERFACE_H
