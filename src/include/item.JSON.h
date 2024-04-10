#ifndef ITEM_H
#define ITEM_H
#define T JSON_Item

typedef struct T T;

typedef enum {
JSON_t_default,
JSON_t_string,
JSON_t_float,
JSON_t_double,
JSON_t_bool,
JSON_t_int,
JSON_t_map,
JSON_t_array,
JSON_t_null,
} JSON_t;

struct T {
  JSON_t type;
  void*value;
};

#undef T
#endif
