#ifndef ISDESTROYABLE_H
#define ISDESTROYABLE_H
#define T JSON_IsDestroyable

typedef struct T {
  void * destructor;
} T;

#undef T
#endif
