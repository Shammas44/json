#ifndef MAP_H
#define MAP_H
#include "Array.JSON.h"
#include "item.JSON.h"
#include <stdbool.h>
#include <stddef.h>
#define T JSON_Hashmap

typedef struct T T;

typedef struct {
  char *key;
  Item_type type;
  void*value;
} JSON_Hashmap_Entry;

typedef void(JSON_Hashmap_destructor)(T *self);
typedef void(JSON_Hashmap_destructor_callback)(void*value);

struct T {
 JSON_IsDestroyable __destructor;
 JSON_Hashmap_destructor *destructor;
 void(*push)(T *self, JSON_Hashmap_Entry entry);
 Item(*get)(T *self, char*key);
 char *(*to_json)(T *self);
 size_t(*capacity)(T *self);
 size_t (*length)(T *self);
 int (*delete)(T *self, char*key);
 Item** (*values)(T *self);
 char** (*keys)(T *self);
 JSON_Hashmap_Entry** (*entries)(T *self);
 void *__private;
};

T *hashmap_constructor(size_t initial_capacity);
void hashmap_destructor(T *self);

#undef T
#endif
