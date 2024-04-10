#ifndef JSON_H
#define JSON_H
#include "Hashmap.JSON.h"
#include "Array.JSON.h"
#include "item.JSON.h"
#include "jsmn.JSON.h"
#include "isDestroyable.JSON.h"

JSON_Item json_parse(char *json);

#endif
