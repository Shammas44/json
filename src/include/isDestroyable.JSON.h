#ifndef ISDESTROYABLE_JSON_H
#define ISDESTROYABLE_JSON_H
#define T JSON_IsDestroyable

typedef struct T {
  void * destructor;
} T;

#undef T
#endif
