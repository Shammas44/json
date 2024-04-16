#ifndef MAP_JSON_H
#define MAP_JSON_H
#include "Array.JSON.h"
#include "item.JSON.h"
#include <stdbool.h>
#include <stddef.h>

#define T JSON_map

#define JSON_MAP_PUSH(map, ...) \
    do { \
        map->push(map, (JSON_Map_Entry){ __VA_ARGS__ }); \
    } while(0)

typedef struct T T;

/**
 * An element of an map of type T
 *   key    key of the element
 *   type   Describe the kind of element of the element
 *   value  The actual value from the element
 */
typedef struct {
  char *key;
  JSON_t type;
  void*value;
} JSON_Map_Entry;

/**
 * Free an map of type T
 *   @param self  The map to free
 */
typedef void(JSON_Map_destructor)(T *self);

/**
 * An heap allocated array datastructure kind
 *   __destructor  Private property (for internal use only)
 *   destructor    Free the map and his values
 *   push          Add an element to the map
 *   get           Retrieve an element from the map
 *   to_json       Retrieve a json representation of the map
 *   capacity      Retrieve the current capacity of the map
 *   length        Retrieve the current length of the hashmp
 *   delete        Remove and free an map element
 *   values        Retrieve a raw array of JSON_Item's
 *                   - The JSON_Item's array need to be free
 *                   - Each JSON_Item's value is a direct reference to the array value, don't free them. 
 *   keys          Retrieve an array of the map's keys
 *                   - The JSON_Item's array need to be free
 *                   - Each JSON_Item's key need to be free
 *   entries       Retrieve an array of keys and values
 *                   - The JSON_Map_Entry's map need to be free
 *                   - Each JSON_Item's key need to be free
 *                   - Each JSON_Map_Entry's value is a direct reference to the map value, don't free them. 
 *   __private     Private property (for internal use only)
 */
struct T {
 JSON_IsDestroyable __destructor;
 JSON_Map_destructor *destructor;
 void(*push)(T *self, JSON_Map_Entry entry);
 JSON_Item(*get)(T *self, char*key);
 char *(*to_json)(T *self);
 size_t(*capacity)(T *self);
 size_t (*length)(T *self);
 int (*delete)(T *self, char*key);
 JSON_Item** (*values)(T *self);
 char** (*keys)(T *self);
 JSON_Map_Entry** (*entries)(T *self);
 void *__private;
};

/**
 * Create an map of type T
 *   @param initial_capacity  The default capacity
 *   @return                  An Heap allocated map of type T
 */
T *JSON_map_constructor(size_t initial_capacity);

#undef T
#endif
