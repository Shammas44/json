#ifndef ITEM_H
#define ITEM_H
#define T Item

typedef struct T T;

typedef enum {
Item_default,
Item_string,
Item_float,
Item_double,
Item_bool,
Item_int,
Item_map,
Item_array,
Item_null,
} Item_type;

struct T {
  Item_type type;
  void*value;
};

#undef T
#endif
