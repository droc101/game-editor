//
// Created by droc101 on 4/21/2024.
//

#include "List.h"
#include <errno.h>
#include <signal.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void ListCreate(List *list)
{
	list->length = 0;
	list->data = NULL;
}

void ListAdd(List *list, void *data)
{
	list->data = GameReallocArray(list->data, list->length + 1, sizeof(void *));
	list->data[list->length] = data;
	list->length++;
}

void ListSet(List *list, const size_t index, void *data)
{
	list->data[index] = data;
}

void ListAddBatched(List *list, const size_t count, ...)
{
	list->data = GameReallocArray(list->data, list->length + count, sizeof(void *));

	va_list args;
	va_start(args, count);
	for (size_t i = 0; i < count; i++)
	{
		list->data[list->length] = va_arg(args, void *);
		list->length++;
	}
	va_end(args);
}

void ListRemoveAt(List *list, const size_t index)
{
	list->length--;
	if (list->length == 0)
	{
		free(list->data);
		list->data = NULL;
		return;
	}
	memmove(&list->data[index], &list->data[index + 1], sizeof(void *) * (list->length - index));
	list->data = GameReallocArray(list->data, list->length, sizeof(void *));
}

void ListInsertAfter(List *list, size_t index, void *data)
{
	index++;
	list->length++;
	list->data = GameReallocArray(list->data, list->length, sizeof(void *));

	memmove(&list->data[index + 1], &list->data[index], sizeof(void *) * (list->length - index - 1));
	list->data[index] = data;
}

size_t ListFind(const List list, const void *data)
{
	if (!list.length || !list.data)
	{
		return -1;
	}

	for (size_t i = 0; i < list.length; i++)
	{
		if (list.data[i] == data)
		{
			return i;
		}
	}
	return -1;
}

void ListClear(List *list)
{
	list->length = 0;
	free(list->data);
	list->data = NULL;
}

void ListFree(List *list, const bool freeListPointer)
{
	free(list->data);
	if (freeListPointer)
	{
		free(list);
	}
}

void ListFreeOnlyContents(const List list)
{
	if (list.length && list.data)
	{
		for (size_t i = 0; i < list.length; i++)
		{
			free(list.data[i]);
		}
	}
}

void ListAndContentsFree(List *list, const bool freeListPointer)
{
	ListFreeOnlyContents(*list);

	ListFree(list, freeListPointer);
}

void *GameReallocArray(void *ptr, const size_t arrayLength, const size_t elementSize)
{
	return realloc(ptr, arrayLength * elementSize);
}
