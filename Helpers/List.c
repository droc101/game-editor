//
// Created by droc101 on 1/13/25.
//

#include "List.h"
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include "../defines.h"

List *CreateList()
{
	List *list = malloc(sizeof(List));
	list->size = 0;
	list->data = calloc(0, sizeof(void *));
	if (list->data == NULL)
	{
		exit(1);
	}
	return list;
}

void ListAdd(List *list, void *data)
{
	list->size++;
	void **temp = GameReallocArray(
			list->data,
			list->size,
			sizeof(void *)); // The size should never be 0 here, so we don't need to check for that
	list->data = temp;
	list->data[list->size - 1] = data;
}

void ListRemoveAt(List *list, const int index)
{
	for (int i = index; i < list->size - 1; i++)
	{
		list->data[i] = list->data[i + 1];
	}
	list->size--;
	void **temp = GameReallocArray(list->data, list->size, sizeof(void *));
	if (list->size == 0 && temp == NULL) // reallocarray with size 0 frees the memory
	{
		temp = malloc(0);
	}
	list->data = temp;
}

void ListInsertAfter(List *list, const int index, void *data)
{
	list->size++;
	void **temp = GameReallocArray(
			list->data,
			list->size,
			sizeof(void *)); // The size should never be 0 here, so we don't need to check for that

	for (int i = list->size - 1; i > index; i--)
	{
		temp[i] = temp[i - 1];
	}
	temp[index + 1] = data;
	list->data = temp;
}

void ListFree(List *list)
{
	free(list->data);
	free(list);
}

void ListFreeWithData(List *list)
{
	for (int i = 0; i < list->size; i++)
	{
		free(list->data[i]);
	}
	ListFree(list);
}

inline int ListGetSize(const List *list)
{
	return list->size;
}

int ListFind(const List *list, const void *data)
{
	for (int i = 0; i < list->size; i++)
	{
		if (list->data[i] == data)
		{
			return i;
		}
	}
	return -1;
}

void ListClear(List *list)
{
	list->size = 0;
	free(list->data);
	list->data = calloc(0, sizeof(void *));
}

void *GameReallocArray(void *ptr, const size_t arrayLength, const size_t elementSize)
{
	if (elementSize == 0)
	{
		return NULL;
	}
	if (arrayLength > SIZE_MAX / elementSize)
	{
		errno = ENOMEM;
		return NULL;
	}
	return realloc(ptr, arrayLength * elementSize);
}
