#ifndef MAP_JSON_H
#define MAP_JSON_H
#include "Array.JSON.h"
#include "item.JSON.h"
#include <stdbool.h>
#include <stddef.h>
#define T JSON_Hashmap

typedef struct T T;

/**
 * An element of an hashmap of type T
 *   key    key of the element
 *   type   Describe the kind of element of the element
 *   value  The actual value from the element
 */
typedef struct {
  char *key;
  JSON_t type;
  void*value;
} JSON_Hashmap_Entry;

/**
 * Free an hashmap of type T
 *   @param self  The hashmap to free
 */
typedef void(JSON_Hashmap_destructor)(T *self);

/**
 * An heap allocated array datastructure kind
 *   __destructor  Private property (for internal use only)
 *   destructor    Free the hashmap and his values
 *   push          Add an element to the hashmap
 *   get           Retrieve an element from the hashmap
 *   to_json       Retrieve a json representation of the hashmap
 *   capacity      Retrieve the current capacity of the hashmap
 *   length        Retrieve the current length of the hashmp
 *   delete        Remove and free an hashmap element
 *   values        Retrieve a raw array of JSON_Item's
 *   keys          Retrieve an array of the hashmap's keys
 *   entries       Retrieve an array of keys and values
 *   __private     Private property (for internal use only)
 */
struct T {
 JSON_IsDestroyable __destructor;
 JSON_Hashmap_destructor *destructor;
 void(*push)(T *self, JSON_Hashmap_Entry entry);
 JSON_Item(*get)(T *self, char*key);
 char *(*to_json)(T *self);
 size_t(*capacity)(T *self);
 size_t (*length)(T *self);
 int (*delete)(T *self, char*key);
 JSON_Item** (*values)(T *self);
 char** (*keys)(T *self);
 JSON_Hashmap_Entry** (*entries)(T *self);
 void *__private;
};

/**
 * Create an hashmap of type T
 *   @param initial_capacity  The default capacity
 *   @return                  An Heap allocated hashmap of type T
 */
T *JSON_hashmap_constructor(size_t initial_capacity);

#undef T
#endif
