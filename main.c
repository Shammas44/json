#include <logger.h>
#include <json.h>
#include <stdio.h>
#include <stdlib.h>
#define T JSON_Hashmap

int main() {
  printf("hello world\n");
  LOG_INFO("info");
  LOG_WARNING("warning");
  LOG_ERROR("error");
  LOG_SUCCESS("success");
  T*map = hashmap_constructor(10);
  T*map2 = hashmap_constructor(10);
  char* value = malloc(sizeof(char)*2);
  sprintf(value,"$");
  JSON_Hashmap_Entry item = {.key="DOLLAR",.type=Item_string,.value=value};
  map2->push(map2,item);
  JSON_Hashmap_Entry item2 = {.key="Symbols",.type=Item_array,.value=map2};
  map->push(map, item2);
  map->destructor(map);
  return 0;
}

#undef T
