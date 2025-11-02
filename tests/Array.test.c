#include <stdio.h>
#include <criterion/criterion.h>
#include <json.h>
#include <logger/logger.h>
#define T JSON_Array

static T *a = NULL;

static void setup(void) {
  LOG_runtimeLogger_reset();
  runtimeLogger_mode = LOG_RuntimeLogger_mode_silent;
}

static void teardown(void) { 
  a->destructor(a);
}

Test(T, constructor_param_check, .init = setup) {
  T*a = JSON_array_constructor(0);
  cr_expect_null(a, "array is not null");
  cr_expect_neq(strcmp(LOG_runtimeLogger_get_last_log().message,"No error"),0);
}

Test(T, constructor_default_length, .fini = teardown, .init = setup) {
  a = JSON_array_constructor(10);
  cr_expect_not_null(a, "array is null");
  cr_expect_eq(a->length(a), 0);
}

Test(T, push_correct_length, .fini = teardown, .init = setup) {
  a = JSON_array_constructor(10);
  char* value = malloc(sizeof(char)*10);
  sprintf(value,"test");
  a->push(a,(JSON_Item){.type=JSON_t_string,.value=value});
  cr_expect_eq(a->length(a), 1);
}

Test(T, push_correct_resizing, .fini = teardown, .init = setup) {
  size_t size = 5;
  a = JSON_array_constructor(size);
  for (size_t i = 0; i < size+1; i++) {
    char* value = malloc(sizeof(char)*10);
    sprintf(value,"test");
    a->push(a,(JSON_Item){.type=JSON_t_string,.value=value});
  }
  cr_expect_eq(a->length(a), size+1);
  cr_expect_gt(a->capacity(a), size);
}

Test(T, get_item_after_resize, .fini = teardown, .init = setup) {
  size_t size = 2;
  a = JSON_array_constructor(size);
  for (size_t i = 0; i < size+1; i++) {
    char* value = malloc(sizeof(char)*10);
    sprintf(value,"test%zu",i);
    a->push(a,(JSON_Item){.type=JSON_t_string,.value=value});
  }
  char* item0 = a->get(a,0).value;
  char* item1 = a->get(a,1).value;
  char* item2 = a->get(a,2).value;
  cr_expect_eq(strcmp(item0,"test0"),0,"values are not equal");
  cr_expect_eq(strcmp(item1,"test1"),0,"values are not equal");
  cr_expect_eq(strcmp(item2,"test2"),0,"values are not equal");
}

Test(T, get_retrieve_items, .fini = teardown, .init = setup) {
  size_t size = 3;
  a = JSON_array_constructor(size);
  for (size_t i = 0; i < size+1; i++) {
    char* value = malloc(sizeof(char)*10);
    sprintf(value,"test %zu",i);
    a->push(a,(JSON_Item){.type=JSON_t_string,.value=value});
  }
  JSON_Item item0 = a->get(a,0);
  cr_expect_eq(strcmp(item0.value,"test 0"),0);
  JSON_Item item1 = a->get(a,1);
  cr_expect_eq(strcmp(item1.value,"test 1"),0);
  JSON_Item item2 = a->get(a,2);
  cr_expect_eq(strcmp(item2.value,"test 2"),0);
}

Test(T, get_outOfBounds_check, .fini = teardown, .init = setup) {
  size_t size = 3;
  a = JSON_array_constructor(size);
  for (size_t i = 0; i < size+1; i++) {
    char* value = malloc(sizeof(char)*10);
    sprintf(value,"test %zu",i);
    a->push(a,(JSON_Item){.type=JSON_t_string,.value=value});
  }
  JSON_Item item0 = a->get(a,4);
  cr_expect_eq(item0.type,JSON_t_null);
  cr_expect_eq(item0.value,NULL);
  JSON_Item item1 = a->get(a,-1);
  cr_expect_eq(item1.type,JSON_t_null);
  cr_expect_eq(item1.value,NULL);
}

Test(T, destructor_is_truly_destroyed, .init = setup) {
  size_t size = 3;
  a = JSON_array_constructor(size);
  char* value0 = malloc(sizeof(char)*10);
  sprintf(value0,"test 0");
  a->push(a,(JSON_Item){.type=JSON_t_string,.value=value0});
  char* value1 = malloc(sizeof(char)*10);
  sprintf(value1,"test 1");
  a->push(a,(JSON_Item){.type=JSON_t_string,.value=value1});
  a->destructor(a);
  cr_expect_neq(strcmp(value0,"value0"),0, "values are the same");
  cr_expect_neq(strcmp(value1,"value1"),0, "values are the same");
}

Test(T, destructor_deep_array, .init = setup) {
  a = JSON_array_constructor(3);
  T*b = JSON_array_constructor(3);
  char* value0 = malloc(sizeof(char)*10);
  sprintf(value0,"test0");
  char* value1 = malloc(sizeof(char)*10);
  sprintf(value1,"test1");
  b->push(b,(JSON_Item){.type=JSON_t_string,.value=value0});
  b->push(b,(JSON_Item){.type=JSON_t_string,.value=value1});
  a->push(a,(JSON_Item){.type=JSON_t_array,.value=b});
  JSON_Item items = a->get(a,0);
  JSON_Array* c = items.value;
  JSON_Item item0 = c->get(c,0);
  JSON_Item item1 = c->get(c,1);
  a->destructor(a);
  cr_expect_neq(strcmp(item0.value,"test0"),0, "values are the same");
  cr_expect_neq(strcmp(item1.value,"test1"),0, "values are the same");
}

Test(T, get_retrieve_array, .fini = teardown, .init = setup) {
  a = JSON_array_constructor(3);
  T*b = JSON_array_constructor(3);
  char* value0 = malloc(sizeof(char)*10);
  sprintf(value0,"test0");
  char* value1 = malloc(sizeof(char)*10);
  sprintf(value1,"test1");
  b->push(b,(JSON_Item){.type=JSON_t_string,.value=value0});
  b->push(b,(JSON_Item){.type=JSON_t_string,.value=value1});
  a->push(a,(JSON_Item){.type=JSON_t_array,.value=b});
  JSON_Item items = a->get(a,0);
  JSON_Array* c = items.value;
  JSON_Item item0 = c->get(c,0);
  JSON_Item item1 = c->get(c,1);
  cr_expect_eq(strcmp(item0.value,"test0"),0, "values are not equal");
  cr_expect_eq(strcmp(item1.value,"test1"),0, "values are not equal");
}

Test(T, keys_correct_keys, .init=setup, .fini = teardown) {
  a = JSON_array_constructor(5);
  int sum = 0;
  int result = 0;
  for (int i = 0; i < 5; i++) {
    sum+=i;
    int * value = malloc(sizeof(int));
    *value = i;
    a->push(a,(JSON_Item){.type=JSON_t_int,.value=value});
  }
  char**keys = a->keys(a);
  for (int i = 0; i < 5; i++) {
    result+= atoi(keys[i]);
  }
  cr_assert_eq(result,sum, "Should be equal");
}

Test(T, values_correct_values, .init=setup, .fini = teardown) {
  a = JSON_array_constructor(5);
  int sumValues = 0;
  int sum = 0;
  for (int i = 0; i < 5; i++) {
    sum+=i;
    int* value = malloc(sizeof(int));
    *value =i;
    a->push(a,(JSON_Item){.type=JSON_t_int,.value=value});
  }
  JSON_Item**values = a->values(a);
  for (int i = 0; i < 5; i++) {
    int *value = values[i]->value;
    sumValues+= *value;
  }
  cr_assert_eq(sumValues,sum, "Should be equal");
}

Test(T, entries_correct_values, .init=setup, .fini = teardown) {
  a = JSON_array_constructor(5);
  int sumValues = 0;
  int sumKeys = 0;
  int sum = 0;
  for (int i = 0; i < 5; i++) {
    sum+=i;
    int* value = malloc(sizeof(int));
    *value =i;
    a->push(a,(JSON_Item){.type=JSON_t_int,.value=value});
  }
  JSON_Array_Entry**values = a->entries(a);
  for (int i = 0; i < 5; i++) {
    int *value = values[i]->value;
    char* key = values[i]->key;
    sumValues+= *value;
    sumKeys+= atoi(key);
    free(key);
  }
  cr_assert_eq(sumValues,sum, "Should be equal");
  cr_assert_eq(sumKeys,sum, "Should be equal");
}

Test(T, to_json_correct_values, .fini = teardown) {
  a = JSON_array_constructor(10);
  char *res = "[\"$\"]";
  JSON_Item item ={.type=JSON_t_string,.value=strdup("$")};
  a->push(a, item);
  char *json = a->to_json(a);
  cr_assert_eq(strcmp(json,res),0, "Should be equal");
}

Test(T, to_json_double_keys, .fini = teardown) {
  a = JSON_array_constructor(10);
  char *res = "[\"$\",\"€\"]";
  JSON_Item item ={.type=JSON_t_string,.value=strdup("$")};
  a->push(a ,item);
  JSON_Item item2 ={.type=JSON_t_string,.value=strdup("€")};
  a->push(a ,item2);
  char *json = a->to_json(a);
  cr_assert_eq(strcmp(json,res),0, "Should be equal");
}

Test(T, to_json_double, .fini = teardown) {
  a = JSON_array_constructor(10);
  char *res = "[2.5,2]";
  double *price = malloc(sizeof(double));
  *price = 2.5;
  double *amount = malloc(sizeof(double));
  *amount = 2;
  JSON_Item item ={.type=JSON_t_double,.value=price};
  a->push(a ,item);
  JSON_Item item2 ={.type=JSON_t_double,.value=amount};
  a->push(a ,item2);
  char *json = a->to_json(a);
  cr_assert_eq(strcmp(json,res),0, "Should be equal");
}

Test(T, to_json_int, .fini = teardown) {
  a = JSON_array_constructor(10);
  char *res = "[3,2]";
  int * price = malloc(sizeof(int));
  int * amount = malloc(sizeof(int));
  *price = 3;
  *amount = 2;
  JSON_Item item ={.type=JSON_t_int,.value=price};
  a->push(a ,item);
  JSON_Item item2 ={.type=JSON_t_int,.value=amount};
  a->push(a ,item2);
  char *json = a->to_json(a);
  cr_assert_eq(strcmp(json,res),0, "Should be equal");
}

Test(T, to_json_boolean, .fini = teardown) {
  a = JSON_array_constructor(10);
  char *res = "[false,true]";
  bool *price = malloc(sizeof(bool));
  bool *amount = malloc(sizeof(bool));
  *price = false;
  *amount = true;
  JSON_Item item ={.type=JSON_t_bool,.value=price};
  a->push(a ,item);
  JSON_Item item2 ={.type=JSON_t_bool,.value=amount};
  a->push(a ,item2);
  char *json = a->to_json(a);
  cr_assert_eq(strcmp(json,res),0, "Should be equal");
}

Test(T, to_json_null, .fini = teardown) {
  a = JSON_array_constructor(10);
  char *res = "[null,null]";
  char* price = NULL;
  char* amount = NULL;
  JSON_Item item ={.type=JSON_t_null,.value=price};
  a->push(a ,item);
  JSON_Item item2 ={.type=JSON_t_null,.value=amount};
  a->push(a ,item2);
  char *json = a->to_json(a);
  cr_assert_eq(strcmp(json,res),0, "Should be equal");
}

Test(T, to_json_map, .fini = teardown) {
  a = JSON_array_constructor(10);
  char *res = "[{\"value\":\"£\"},{\"value\":\"$\"}]";
  JSON_Map* price = JSON_map_constructor(1);
  JSON_Map_Entry item ={.key="value",.type=JSON_t_string,.value=strdup("£")};
  price->push(price ,item);
  JSON_Map* amount = JSON_map_constructor(1);
  JSON_Map_Entry item2 ={.key="value",.type=JSON_t_string,.value=strdup("$")};
  amount->push(amount ,item2);
  JSON_Item item3 ={.type=JSON_t_map,.value=price};
  a->push(a ,item3);
  JSON_Item item4 ={.type=JSON_t_map,.value=amount};
  a->push(a ,item4);
  char *json = a->to_json(a);
  cr_assert_eq(strcmp(json,res),0, "Should be equal");
}

Test(T, to_json_array, .fini = teardown) {
  a = JSON_array_constructor(10);
  char *res = "[[\"£\",\"$\"],[\"£\",\"$\"]]";
  T*b = JSON_array_constructor(2);
  T*c = JSON_array_constructor(2);
  JSON_Item item1 ={.type=JSON_t_string,.value=strdup("£")};
  JSON_Item item2 ={.type=JSON_t_string,.value=strdup("$")};
  JSON_Item item3 ={.type=JSON_t_string,.value=strdup("£")};
  JSON_Item item4 ={.type=JSON_t_string,.value=strdup("$")};
  b->push(b ,item1);
  b->push(b ,item2);
  c->push(c ,item3);
  c->push(c ,item4);
  a->push(a,(JSON_Item){.type=JSON_t_array,.value=b});
  a->push(a,(JSON_Item){.type=JSON_t_array,.value=c});
  char *json = a->to_json(a);
  cr_assert_eq(strcmp(json,res),0, "Should be equal");
}

Test(T, delete, .fini=teardown ) {
  a = JSON_array_constructor(3);
  JSON_Item item1 ={.type=JSON_t_string,.value=strdup("1")};
  a->push(a ,item1);
  JSON_Item item2 ={.type=JSON_t_string,.value=strdup("2")};
  a->push(a ,item2);
  JSON_Item item3 ={.type=JSON_t_string,.value=strdup("3")};
  a->push(a ,item3);
  int status = a->delete(a,"1");
  JSON_Item item = a->get(a,1);
  cr_assert_eq(a->length(a), 2, "Should be equal");
  cr_assert_null(item.value, "Should be null");
  cr_assert_eq(status,0, "Should be equal");
}
