#ifndef PARSER_JSON_H
#define PARSER_JSON_H
#include "item.JSON.h"

/**
 * A function to parse a json string
 *   @param json  a json string to parse
 *   @return      an item either of type JSON_t_map or JSON_t_array
 */
JSON_Item JSON_parse(char *json);

#endif
