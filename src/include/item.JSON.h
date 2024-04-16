#ifndef ITEM_JSON_H
#define ITEM_JSON_H
#define T JSON_Item

typedef struct T T;

/**
 * An enum representing any type of data representable in a json string
 */
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

/**
 * A struct to represent a value return from an JSON_Array or JSON_map
 *   type   the type of data of the item
 *   value  the actual value of the item
 */
struct T {
  JSON_t type;
  void *value;
};

#undef T
#endif
