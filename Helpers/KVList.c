//
// Created by droc101 on 5/28/25.
//

#include "KVList.h"

#include <stdlib.h>
#include <string.h>

#pragma region Private Functions

/**
 * Get the index of a key in the key-value list.
 * @param list The list to search in.
 * @param key The key to search for.
 * @return The index of the key in the list, or -1 if not found.
 * @todo This should be optimized.
 */
size_t KvIndexOf(const KvList *list, const char *key)
{
	for (size_t i = 0; i < list->keys.length; i++)
	{
		if (strcmp(ListGet(list->keys, i), key) == 0)
		{
			return i;
		}
	}
	return -1; // Not found
}

/**
 * Get a value from the kvlist by key, with a default value if the key does not exist or is of the wrong type
 * @param list The list to get the value from
 * @param key The key to get the value for
 * @param expectedType The expected type of the value
 * @param defaultValue The default value to return if the key does not exist or is of the wrong type
 * @return The value associated with the key, or the default value if not found or of the wrong type
 */
Param *KvGetTypeWithDefault(const KvList *list, const char *key, const ParamType expectedType, Param *defaultValue)
{
	if (!list || !key)
	{
		return defaultValue;
	}
	Param *p = KvGet(list, key);
	if (!p || p->type != expectedType)
	{
		return defaultValue;
	}
	return p;
}

#pragma endregion

void KvListCreate(KvList *list)
{
	if (!list)
	{
		return;
	}
	ListCreate(&list->keys);
	ListCreate(&list->values);
}

void KvListDestroy(KvList *list)
{
	if (!list)
	{
		return;
	}

	ListAndContentsFree(&list->keys, false);
	ListAndContentsFree(&list->values, false);
}

Param *KvGet(const KvList *list, const char *key)
{
	if (!list || !key)
	{
		return NULL;
	}
	const size_t index = KvIndexOf(list, key);
	if (index != -1)
	{
		return ListGet(list->values, index);
	}
	return NULL; // Not found
}

void KvSet(KvList *list, const char *key, const Param value)
{
	if (!list || !key)
	{
		return;
	}
	const size_t index = KvIndexOf(list, key);
	Param *p = malloc(sizeof(Param));
	*p = value;
	char *k = strdup(key);
	if (index != -1)
	{
		ListSet(&list->values, index, p);
	} else
	{
		ListAdd(&list->keys, k);
		ListAdd(&list->values, p);
	}
}

void KvDelete(KvList *list, const char *key)
{
	if (!list || !key)
	{
		return;
	}
	const size_t index = KvIndexOf(list, key);
	if (index != -1)
	{
		ListRemoveAt(&list->keys, index);
		ListRemoveAt(&list->values, index);
	}
}

size_t KvListLength(const KvList *list)
{
	if (!list)
	{
		return 0;
	}
	return list->keys.length;
}

bool KvListHas(const KvList *list, const char *key)
{
	if (!list || !key)
	{
		return false;
	}
	const size_t index = KvIndexOf(list, key);
	return index != -1;
}

ParamType KvGetType(const KvList *list, const char *key)
{
	Param *p = KvGet(list, key);
	if (!p)
	{
		return PARAM_TYPE_NONE; // Key not found
	}
	return p->type;
}

char *KvListGetKeyName(const KvList *list, const size_t index)
{
	if (!list || index >= list->keys.length)
	{
		return NULL;
	}
	return ListGet(list->keys, index);
}

#pragma region Public Getters

byte KvGetByte(const KvList *list, const char *key, const byte defaultValue)
{
	const Param *p = KvGetTypeWithDefault(list,
										  key,
										  PARAM_TYPE_BYTE,
										  &(Param){PARAM_TYPE_BYTE, .byteValue = defaultValue});
	if (p)
	{
		return p->byteValue;
	}
	return defaultValue;
}

int KvGetInt(const KvList *list, const char *key, const int defaultValue)
{
	const Param *p = KvGetTypeWithDefault(list,
										  key,
										  PARAM_TYPE_INTEGER,
										  &(Param){PARAM_TYPE_INTEGER, .intValue = defaultValue});
	if (p)
	{
		return p->intValue;
	}
	return defaultValue;
}

float KvGetFloat(const KvList *list, const char *key, const float defaultValue)
{
	const Param *p = KvGetTypeWithDefault(list,
										  key,
										  PARAM_TYPE_FLOAT,
										  &(Param){PARAM_TYPE_FLOAT, .floatValue = defaultValue});
	if (p)
	{
		return p->floatValue;
	}
	return defaultValue;
}

bool KvGetBool(const KvList *list, const char *key, const bool defaultValue)
{
	const Param *p = KvGetTypeWithDefault(list,
										  key,
										  PARAM_TYPE_BOOL,
										  &(Param){PARAM_TYPE_BOOL, .boolValue = defaultValue});
	if (p)
	{
		return p->boolValue;
	}
	return defaultValue;
}

const char *KvGetString(const KvList *list, const char *key, const char *defaultValue)
{
	const Param *p = KvGetTypeWithDefault(list, key, PARAM_TYPE_STRING, &(Param){PARAM_TYPE_STRING, .stringValue = ""});
	if (p)
	{
		return p->stringValue[0] ? p->stringValue : defaultValue;
	}
	return defaultValue;
}

Color KvGetColor(const KvList *list, const char *key, const Color defaultValue)
{
	const Param *p = KvGetTypeWithDefault(list,
										  key,
										  PARAM_TYPE_COLOR,
										  &(Param){PARAM_TYPE_COLOR, .colorValue = defaultValue});
	if (p)
	{
		return p->colorValue;
	}
	return defaultValue;
}

#pragma endregion

#pragma region Public Setters

inline void KvSetByte(KvList *list, const char *key, const byte value)
{
	KvSet(list, key, (Param){PARAM_TYPE_BYTE, .byteValue = value});
}

inline void KvSetInt(KvList *list, const char *key, const int value)
{
	KvSet(list, key, (Param){PARAM_TYPE_INTEGER, .intValue = value});
}

inline void KvSetFloat(KvList *list, const char *key, const float value)
{
	KvSet(list, key, (Param){PARAM_TYPE_FLOAT, .floatValue = value});
}

inline void KvSetBool(KvList *list, const char *key, const bool value)
{
	KvSet(list, key, (Param){PARAM_TYPE_BOOL, .boolValue = value});
}

void KvSetString(KvList *list, const char *key, const char *value)
{
	if (!value)
	{
		value = "";
	}
	KvSet(list, key, (Param){PARAM_TYPE_STRING, .stringValue = ""});
	strncpy(KvGet(list, key)->stringValue, value, sizeof(KvGet(list, key)->stringValue) - 1);
}

inline void KvSetColor(KvList *list, const char *key, const Color value)
{
	KvSet(list, key, (Param){PARAM_TYPE_COLOR, .colorValue = value});
}

#pragma endregion
