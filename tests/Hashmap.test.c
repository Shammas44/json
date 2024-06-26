#include <criterion/criterion.h>
#include <json.h>
#include <stdio.h>
#define T JSON_Map

static T *map = NULL;
static void setup(void) { map = JSON_map_constructor(10); }

static void teardown(void) { 
  map->destructor(map);
}

Test(T, constructor_build, .init = setup, .fini = teardown) {
  cr_assert_not_null(map, "should not be NULL");
  cr_assert_gt(map->capacity(map), 10, "Capacity should be greater");
  cr_assert_eq(map->length(map), 0, "Length should be 0");
}

Test(T, constructor_param_check) {
  map = JSON_map_constructor(0);
  cr_assert_null(map, "should be NULL");
  map = JSON_map_constructor(-1);
  cr_assert_null(map, "should be NULL");
  map = JSON_map_constructor(1000000);
  cr_assert_null(map, "map should be NULL");
}

Test(T, push_insertion, .init = setup, .fini = teardown) {
  char key0[10] = "DOLLAR", value0[10] = "$";
  JSON_Map_Entry item = {
      .key = key0, .type = JSON_t_string, .value = strdup(value0)};
  map->push(map, item);
  cr_assert_eq(map->length(map), 1, "Wrong length");
  char key1[10] = "Euro", value1[10] = "€";
  JSON_Map_Entry item2 = {
      .key = key1, .type = JSON_t_string, .value = strdup(value1)};
  map->push(map, item2);
  cr_assert_eq(map->length(map), 2, "Wrong length");
}

Test(T, get_retrieve, .init = setup, .fini = teardown) {
  char key0[10] = "DOLLAR", value0[10] = "$";
  JSON_Map_Entry item = {
      .key = key0, .type = JSON_t_string, .value = strdup(value0)};
  map->push(map, item);
  char *v0 = map->get(map, key0).value;
  cr_assert_eq(strcmp(v0, value0), 0, "Should be equal");
  char key1[10] = "Euro", value1[10] = "€";
  JSON_Map_Entry item2 = {
      .key = key1, .type = JSON_t_string, .value = strdup(value1)};
  map->push(map, item2);
  char *v1 = map->get(map, key1).value;
  cr_assert_eq(strcmp(v1, value1), 0, "Should be equal");
}

Test(T, get_case_sensitive, .init = setup, .fini = teardown) {
  char key[10] = "DOLLAR", value[10] = "$";
  JSON_Map_Entry item = {
      .key = key, .type = JSON_t_string, .value = strdup(value)};
  map->push(map, item);
  char *result = map->get(map, "dollar").value;
  cr_assert_null(result, "Result should be NULL");
}

Test(T, push_is_resized, .init = setup, .fini = teardown) {
  char key[3];
  for (int i = 0; i < 20; i++) {
    sprintf(key, "%d", i);
    JSON_Map_Entry item = {.key = key, .type = JSON_t_null, .value = NULL};
    map->push(map, item);
  }
  cr_assert_gt(map->capacity(map), 20, "Capacity should be greater");
}

Test(T, push_nested_map, .init = setup, .fini = teardown) {
  T *map2 = JSON_map_constructor(10);
  JSON_Map_Entry item = {
      .key = "DOLLAR", .type = JSON_t_string, .value = strdup("$")};
  map2->push(map2, item);
  JSON_Map_Entry item2 = {
      .key = "Symbols", .type = JSON_t_array, .value = map2};
  map->push(map, item2);
  T *map3 = map->get(map, "Symbols").value;
  char *result = map3->get(map3, "DOLLAR").value;
  cr_assert_eq(strcmp(result, "$"), 0, "Should be equal");
}

Test(T, push_replacement, .init = setup, .fini = teardown) {
  JSON_Map_Entry item = {
      .key = "Symbols", .type = JSON_t_string, .value = strdup("$")};
  map->push(map, item);
  JSON_Map_Entry item2 = {
      .key = "Symbols", .type = JSON_t_string, .value = strdup("€")};
  map->push(map, item2);
  char *result = map->get(map, "Symbols").value;
  cr_assert_eq(strcmp(result, "€"), 0, "Should be equal");
}

Test(T, destructor_is_truly_destroyed) {
  T *map = JSON_map_constructor(10);
  T *map2 = JSON_map_constructor(10);
  char *value = malloc(sizeof(char) * 2);
  sprintf(value, "$");
  JSON_Map_Entry item = {
      .key = "DOLLAR", .type = JSON_t_string, .value = value};
  map2->push(map2, item);
  JSON_Map_Entry item2 = {
      .key = "Symbols", .type = JSON_t_array, .value = map2};
  map->push(map, item2);
  map->destructor(map);
  cr_expect_neq(strcmp(value, "$"), 0, "values are the same");
}

Test(T, keys_correct_keys, .init = setup, .fini = teardown) {
  char key[3];
  int result = 1 + 2 + 3 + 4;
  int sum = 0;
  for (int i = 0; i < 5; i++) {
    sprintf(key, "%d", i);
    JSON_Map_Entry item = {.key = key, .type = JSON_t_null, .value = NULL};
    map->push(map, item);
  }
  char **keys = map->keys(map);
  for (int i = 0; i < 5; i++) {
    sum += atoi(keys[i]);
  }
  cr_assert_eq(result, sum, "Should be equal");
}

Test(T, values_correct_values, .init = setup, .fini = teardown) {
  char key[3];
  int result = 1 + 2 + 3 + 4;
  int sum = 0;
  for (int i = 0; i < 5; i++) {
    sprintf(key, "%d", i);
    char *value = malloc(sizeof(char) * 3);
    sprintf(value, "%d", i);
    JSON_Map_Entry item = {.key = key, .type = JSON_t_null, .value = value};
    map->push(map, item);
  }
  JSON_Item **values = map->values(map);
  for (int i = 0; i < 5; i++) {
    sum += atoi((char *)values[i]->value);
  }
  cr_assert_eq(result, sum, "Should be equal");
}

Test(T, entries_empty_map, .init = setup, .fini = teardown) {
  JSON_Map_Entry **entries = map->entries(map);
  cr_assert_null(entries, "Should be null");
}

Test(T, entries_simple, .init = setup, .fini = teardown) {
  JSON_Map_Entry item = {
      .key = "DOLLAR", .type = JSON_t_string, .value = strdup("$")};
  map->push(map, item);
  JSON_Map_Entry item2 = {
      .key = "EURO", .type = JSON_t_string, .value = strdup("€")};
  map->push(map, item2);
  JSON_Map_Entry **entries = map->entries(map);
  cr_assert_eq(strcmp(entries[0]->value, "$"), 0, "Should be equal");
  cr_assert_eq(strcmp(entries[1]->value, "€"), 0, "Should be equal");
}

Test(T, to_json_correct_values, .fini = teardown) {
  map = JSON_map_constructor(10);
  char *res = "{\"DOLLAR\":\"$\"}";
  JSON_Map_Entry item = {
      .key = "DOLLAR", .type = JSON_t_string, .value = strdup("$")};
  map->push(map, item);
  char *json = map->to_json(map);
  cr_assert_eq(strcmp(json, res), 0, "Should be equal");
}

Test(T, to_json_double_keys, .fini = teardown) {
  map = JSON_map_constructor(10);
  char *res = "{\"DOLLAR\":\"$\",\"EURO\":\"€\"}";
  JSON_Map_Entry item = {
      .key = "DOLLAR", .type = JSON_t_string, .value = strdup("$")};
  map->push(map, item);
  JSON_Map_Entry item2 = {
      .key = "EURO", .type = JSON_t_string, .value = strdup("€")};
  map->push(map, item2);
  char *json = map->to_json(map);
  cr_assert_eq(strcmp(json, res), 0, "Should be equal");
}

Test(T, to_json_double, .fini = teardown) {
  map = JSON_map_constructor(10);
  char *res = "{\"amount\":2,\"price\":2.5}";
  double *price = malloc(sizeof(double));
  double *amount = malloc(sizeof(double));
  *price = 2.5;
  *amount = 2;
  JSON_Map_Entry item = {
      .key = "price", .type = JSON_t_double, .value = price};
  map->push(map, item);
  JSON_Map_Entry item2 = {
      .key = "amount", .type = JSON_t_double, .value = amount};
  map->push(map, item2);
  char *json = map->to_json(map);
  cr_assert_eq(strcmp(json, res), 0, "Should be equal");
}

Test(T, to_json_int, .fini = teardown) {
  map = JSON_map_constructor(10);
  char *res = "{\"amount\":2,\"price\":2}";
  int *price = malloc(sizeof(int));
  int *amount = malloc(sizeof(int));
  *price = 2;
  *amount = 2;
  JSON_Map_Entry item = {.key = "price", .type = JSON_t_int, .value = price};
  map->push(map, item);
  JSON_Map_Entry item2 = {
      .key = "amount", .type = JSON_t_int, .value = amount};
  map->push(map, item2);
  char *json = map->to_json(map);
  cr_assert_eq(strcmp(json, res), 0, "Should be equal");
}

Test(T, to_json_boolean, .fini = teardown) {
  map = JSON_map_constructor(10);
  char *res = "{\"amount\":true,\"price\":false}";
  bool *price = malloc(sizeof(bool));
  bool *amount = malloc(sizeof(bool));
  *price = false;
  *amount = true;
  JSON_Map_Entry item = {.key = "price", .type = JSON_t_bool, .value = price};
  map->push(map, item);
  JSON_Map_Entry item2 = {
      .key = "amount", .type = JSON_t_bool, .value = amount};
  map->push(map, item2);
  char *json = map->to_json(map);
  cr_assert_eq(strcmp(json, res), 0, "Should be equal");
}

Test(T, to_json_null, .fini = teardown) {
  map = JSON_map_constructor(10);
  char *res = "{\"amount\":null,\"price\":null}";
  char *price = NULL;
  char *amount = NULL;
  JSON_Map_Entry item = {.key = "price", .type = JSON_t_null, .value = price};
  map->push(map, item);
  JSON_Map_Entry item2 = {
      .key = "amount", .type = JSON_t_null, .value = amount};
  map->push(map, item2);
  char *json = map->to_json(map);
  cr_assert_eq(strcmp(json, res), 0, "Should be equal");
}

Test(T, to_json_map, .fini = teardown) {
  map = JSON_map_constructor(10);
  char *res = "{\"amount\":{\"value\":\"$\"},\"price\":{\"value\":\"£\"}}";
  JSON_Map *price = JSON_map_constructor(1);
  JSON_Map_Entry item = {
      .key = "value", .type = JSON_t_string, .value = strdup("£")};
  price->push(price, item);
  JSON_Map *amount = JSON_map_constructor(1);
  JSON_Map_Entry item2 = {
      .key = "value", .type = JSON_t_string, .value = strdup("$")};
  amount->push(amount, item2);
  JSON_Map_Entry item3 = {.key = "price", .type = JSON_t_map, .value = price};
  map->push(map, item3);
  JSON_Map_Entry item4 = {
      .key = "amount", .type = JSON_t_map, .value = amount};
  map->push(map, item4);
  char *json = map->to_json(map);
  cr_assert_eq(strcmp(json, res), 0, "Should be equal");
}

Test(T, to_json_array, .fini = teardown) {
  map = JSON_map_constructor(10);
  char *res = "{\"price\":[\"$\",\"£\"]}";
  JSON_Array *array = JSON_array_constructor(2);
  array->push(array, (JSON_Item){.type = JSON_t_string, .value = strdup("$")});
  array->push(array, (JSON_Item){.type = JSON_t_string, .value = strdup("£")});
  map->push(map, (JSON_Map_Entry){
                     .key = "price", .type = JSON_t_array, .value = array});
  char *json = map->to_json(map);
  cr_assert_eq(strcmp(json, res), 0, "Should be equal");
}

Test(T, delete, .fini = teardown) {
  map = JSON_map_constructor(2);
  char key[3];
  for (int i = 0; i < 2; i++) {
    sprintf(key, "%d", i);
    char *value = malloc(sizeof(char) * 3);
    sprintf(value, "%d", i);
    JSON_Map_Entry item = {.key = key, .type = JSON_t_string, .value = value};
    map->push(map, item);
  }
  int status = map->delete (map, "1");
  size_t length = map->length(map);
  cr_assert_eq(status, 1, "Wrong status code");
  cr_assert_eq(length, 1, "Wrong length");
  JSON_Item item = map->get(map, "1");
  cr_assert_null(item.value, "Should be null");
}
