#ifndef ARRAY_H
#define ARRAY_H
#include <stddef.h>
#include "item.JSON.h"
#include "isDestroyable.JSON.h"

#define T JSON_Array

typedef struct T T;

typedef struct {
  char *key;
  Item_type type;
  void*value;
} JSON_Array_Entry;

typedef void(JSON_Array_destructor)(T *self);
typedef void(JSON_Array_destructor_callback)(void*value);

struct T {
 JSON_IsDestroyable __destructor;
 JSON_Array_destructor *destructor;
 void(*push)(T *self, Item item);
 Item(*get)(T *self, size_t index);
 char *(*to_json)(T *self);
 size_t(*capacity)(T *self);
 size_t (*length)(T *self);
 Item** (*values)(T *self);
 char** (*keys)(T *self);
 JSON_Array_Entry** (*entries)(T *self);
 void *__private;
};

T *array_constructor(size_t initial_size);
void array_destructor(T* self);

#undef T
#endif
