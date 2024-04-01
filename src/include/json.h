#ifndef JSON_H
#define JSON_H
#include "Hashmap.JSON.h"
#include "Array.JSON.h"
#include "item.JSON.h"
#include "jsmn.JSON.h"
#include "isDestroyable.JSON.h"

//TODO remove json_to_array and json_to_map
int json_to_array(char *json, JSON_Array**array, jsmntok_t*tokens,int token_num);
int json_to_map(char *json, JSON_Hashmap**map,jsmntok_t *tokens, int token_num);
void* json_parse(char *json);

#endif
