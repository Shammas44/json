#ifndef ARRAY_H
#define ARRAY_H
#include <stddef.h>
#include "item.JSON.h"
#include "isDestroyable.JSON.h"

#define T JSON_Array

typedef struct T T;

/**
 * A cell of an array of type T
 *   key    Index of the cell (integer in a string format)
 *   type   Describe the kind of element in the cell
 *   value  The actual value from the cell
 */
typedef struct {
  char *key;
  JSON_t type;
  void*value;
} JSON_Array_Entry;

/**
 * Free an array of type T
 *   @param self  The array to free
 */
typedef void(JSON_Array_destructor)(T *self);

/**
 * An heap allocated array datastructure kind
 *   __destructor  Private property (for internal use only)
 *   destructor    Free the array and his values
 *   push          Add an element to the array
 *   get           Retrieve an element from the array
 *   to_json       Retrieve a json representation of the array
 *   capacity      Retrieve the current capacity of the array
 *   length        Retrieve the current length of the array
 *   delete        Remove and free an array cell
 *   values        Retrieve a raw array of JSON_Item's
 *                   - The JSON_Item's array need to be free after usage
 *                   - Each JSON_Item's value is a direct reference to the array value, don't free them. 
 *   keys          Retrieve an array of the array's keys
 *                   - The JSON_Item's array need to be free
 *                   - Each JSON_Item's key need to be free
 *   entries       Retrieve an array of keys and values
 *                   - The JSON_Array_Entry's array need to be free
 *                   - Each JSON_Array_Entry's key need to be free
 *                   - Each JSON_Array_Entry value is a direct reference to the array value, don't free them. 
 *   __private     Private property (for internal use only)
 */
struct T {
 JSON_IsDestroyable __destructor;
 JSON_Array_destructor *destructor;
 void(*push)(T *self, JSON_Item item);
 JSON_Item(*get)(T *self, size_t index);
 char *(*to_json)(T *self);
 size_t(*capacity)(T *self);
 size_t (*length)(T *self);
 int (*delete)(T *self, char*key);
 JSON_Item** (*values)(T *self);
 char** (*keys)(T *self);
 JSON_Array_Entry** (*entries)(T *self);
 void *__private;
};

/**
 * Create an array of type T
 *   @param initial_size  The default array size
 *   @return              An Heap allocated array of type T
 */
T *JSON_array_constructor(size_t initial_size);

#undef T
#endif
