#ifndef ISDESTROYABLE_JSON_H
#define ISDESTROYABLE_JSON_H
#define T JSON_IsDestroyable

/**
 * A struct to keep a destructor function 
 * Any struct implementing this struct as their first property can be casted as a T type
 * This tricks allow to free any heap allocated struct whithout knowing their exact type
 *   destructor  Supposed to be a function pointer used to free an element
 */
typedef struct T {
  void * destructor;
} T;

#undef T
#endif
