#include <pthread.h>
#include <logger/logger.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Array.JSON.h"
#include "Map.JSON.h"
#define T JSON_Array

typedef struct {
  JSON_Item **array;
  size_t length;
  size_t capacity;
  pthread_mutex_t mutex;
} Private;

static JSON_Item __get(T *self, size_t index);
static void __push(T *self, JSON_Item item);
static char *__to_json(T *self);
static size_t __length(T *self);
static int __delete(T *self, char*key);
static size_t __capacity(T *self);
static void __destructor(T *self);
static JSON_Item** __values(T *self);
static char** __keys(T *self);
static JSON_Array_Entry** __entries(T *self);

static char* _$convertToTrimmedString(double num);
static JSON_Item _$get(T *self, size_t index);
static char *_$to_json(T *self);
static JSON_Item** _$values(T *self);
static char** _$keys(T *self);
static JSON_Array_Entry** _$entries(T *self);

static JSON_IsDestroyable d = {
  .destructor = __destructor
};

T *JSON_array_constructor(size_t size) {
  T *self = malloc(sizeof(T));
  if(size <= 0){
    LOG_ERROR("JSON_Array size should be greater than 0");
    return NULL;
  }
  self->length = 0;
  self->__private = malloc(sizeof(Private));
  JSON_Item **array = malloc(sizeof(JSON_Item *) * size);

  for (size_t i = 0; i < size; i++) {
    array[i] = NULL;
  }

  Private *p = self->__private;
  p->array = array;
  p->length = 0;
  p->capacity = size;
  pthread_mutex_init(&p->mutex, NULL);
  self->__destructor = d;
  self->destructor = __destructor;
  self->get = __get;
  self->length = __length;
  self->capacity = __capacity;
  self->delete = __delete;
  self->push = __push;
  self->to_json = __to_json;
  self->keys = __keys;
  self->entries = __entries;
  self->values = __values;
  return self;
}

static void __destructor(T *self) {
  if(self == NULL){
    return;
  }
  Private *p = self->__private;
  pthread_mutex_lock(&p->mutex);
  size_t length = p->length;
  for (size_t i=0;i<length;i++) {
    JSON_Item *entry = p->array[i];
    if(!entry) break;
    JSON_t type = entry->type;
    void * value = entry->value;
    JSON_Map* map;
    JSON_Array* array;
    switch (type) {
      case JSON_t_null:
      break;
      case JSON_t_array:
        array = value;
        array->destructor(array);
      break;
      case JSON_t_map:
        map = value;
        map->destructor(map);
      break;
      default: 
      free(value);
      break;
    
    }
    free(entry);
  }
  free(p->array);
  pthread_mutex_unlock(&p->mutex);
  pthread_mutex_destroy(&p->mutex);
  free(self->__private);
  free(self);
}

static size_t __length(T *self) {
  Private *p = self->__private;
  pthread_mutex_lock(&p->mutex);
  size_t value = p->length;
  pthread_mutex_unlock(&p->mutex);
  return value;
}

static size_t __capacity(T *self){
  Private *p = self->__private;
  pthread_mutex_lock(&p->mutex);
  size_t value = p->capacity;
  pthread_mutex_unlock(&p->mutex);
  return value;
}

static int __delete(T *self, char*key){
  Private *p = self->__private;
  pthread_mutex_lock(&p->mutex);
  int index = atoi(key);
  JSON_Item *entry = p->array[index];
  if(entry== NULL){
    pthread_mutex_unlock(&p->mutex);
    return 1;
  }
  JSON_t type = entry->type;
  void * value = entry->value;
  JSON_Map* map;
  JSON_Array* array;
  switch (type) {
    case JSON_t_null:
      break;
    case JSON_t_array:
      array = value;
      array->destructor(array);
      break;
    case JSON_t_map:
      map = value;
      map->destructor(map);
      break;
    default: 
      free(value);
      break;
  }
  p->length = p->length - 1;
  free(entry);
  p->array[index] = NULL;
  pthread_mutex_unlock(&p->mutex);
  return 0;
}

static JSON_Item __get(T *self, size_t index) {
  Private *p = self->__private;
  pthread_mutex_lock(&p->mutex);
  JSON_Item value = _$get(self, index);
  pthread_mutex_unlock(&p->mutex);
  return value;
}

static JSON_Array_Entry** __entries(T *self) {
  Private *p = self->__private;
  pthread_mutex_lock(&p->mutex);
  JSON_Array_Entry **value = _$entries(self);
  pthread_mutex_unlock(&p->mutex);
  return value;
}

static void __push(T *self, JSON_Item item) {
  Private *p = self->__private;
  pthread_mutex_lock(&p->mutex);
  JSON_Item *entry = malloc(sizeof(JSON_Item));
  JSON_t type = item.type;
  entry->type = item.value ==NULL
  ? JSON_t_null
  : type;
  entry->value = item.value;
  p->array[p->length] = entry;
  p->length++;
  if (p->length == p->capacity) {
     p->capacity = p->capacity * 2;
     p->array = realloc(p->array, sizeof(JSON_Item *) * p->capacity);
  }
  pthread_mutex_unlock(&p->mutex);
}

static JSON_Item** __values(T *self) {
  Private *p = self->__private;
  pthread_mutex_lock(&p->mutex);
  JSON_Item **value = _$values(self);
  pthread_mutex_unlock(&p->mutex);
  return value;
}

static char** __keys(T *self) {
  Private *p = self->__private;
  pthread_mutex_lock(&p->mutex);
  char **value = _$keys(self);
  pthread_mutex_unlock(&p->mutex);
  return value;
}

static char* __to_json(T* self) {
  Private *p = self->__private;
  pthread_mutex_lock(&p->mutex);
  char *value = _$to_json(self);
  pthread_mutex_unlock(&p->mutex);
  return value;
}

static char* _$to_json(T* self) {
  if (self == NULL) {
    return NULL;
  }

  const size_t initialCapacity = 100000;
  char* json = malloc(sizeof(char) * initialCapacity);
  if (json == NULL) {
    return NULL;  // Memory allocation failure
  }

  strcpy(json, "[");

  Private* p = self->__private;
  size_t length = p->length;
  JSON_Item** values = _$values(self);

  for (size_t i = 0; i < length; i++) {
    char* value = NULL;
    char* tick = "\"";
    char* comma = (i == length - 1) ? "" : ",";
    JSON_Map*map;

    switch (values[i]->type) {
      case JSON_t_string:
      case JSON_t_default:
        value = values[i]->value;
        break;

      case JSON_t_double:
        value = _$convertToTrimmedString(*(double*)values[i]->value);
        tick = "";
        break;

      case JSON_t_int:
        value = malloc(sizeof(char) * 100);
        if (value != NULL) {
          sprintf(value, "%d", *(int*)values[i]->value);
          tick = "";
        } else {
          free(json);
          return NULL;  // Memory allocation failure
        }
        break;

      case JSON_t_bool:
        value = *(bool*)values[i]->value ? "true" : "false";
        tick = "";
        break;

      case JSON_t_null:
        value = "null";
        tick = "";
        break;

      case JSON_t_array:
        value = _$to_json(values[i]->value);
        tick = "";
        break;
      case JSON_t_map:
        map = values[i]->value;
        value = map->to_json(map);
        tick = "";
        break;

      default:
        // Handle unsupported type or error
        free(json);
        return NULL;
    }

    strcat(json, tick);
    strcat(json, value);
    strcat(json, tick);
    strcat(json, comma);

    JSON_t to_free[4] = {JSON_t_int,JSON_t_double,JSON_t_array,JSON_t_map};
    for (int j=0; j<4; j++) {
    if (values[i]->type == to_free[j]) {
        free(value);
        break;
      }
    }
  }

  strcat(json, "]");
  for (size_t i =0;i<length;i++) {
    free(values[i]);
  }
  free(values);
  return json;
}

static char* _$convertToTrimmedString(double num) {
    char buffer[50];  // Adjust the size based on your needs
    sprintf(buffer, "%.15f", num);  // Use a precision that fits your requirements

    // Remove trailing zeroes
    int len = strlen(buffer);
    while (len > 1 && buffer[len - 1] == '0') {
        buffer[--len] = '\0';
    }

    // Remove trailing decimal point, if present
    if (buffer[len - 1] == '.') {
        buffer[--len] = '\0';
    }

    // Allocate memory for the final string and copy the result
    char *result = strdup(buffer);

    return result;
}

static JSON_Item _$get(T *self, size_t index) {
  Private *p = self->__private;
  if (index >= p->length) {
    LOG_ERROR("index out of bounds");
    return (JSON_Item){.type=JSON_t_null,.value=NULL};
  }
  JSON_Item *entry = p->array[index];
  if(entry == NULL) return (JSON_Item){.type=JSON_t_null,.value=NULL}; 
  JSON_Item item = {.type=entry->type,.value=entry->value};
  return item;
}

static JSON_Item** _$values(T *self){
  Private *p = self->__private;
  size_t length = p->length;
  if(length == 0) return NULL;
  JSON_Item **output = malloc(sizeof(JSON_Item *) * length);
  if (output == NULL) return NULL;

  for (size_t i=0; i < length; i++) {
    JSON_Item item = _$get(self, i); 
    output[i] = malloc(sizeof(JSON_Item));
    output[i]->value = item.value;
    output[i]->type = item.type;
  }
  return output;
}

static char** _$keys(T *self){
  Private *p = self->__private;
  size_t length = p->length;
  char **output = malloc(sizeof(char *) * length);
  size_t j = 0;
  if (output == NULL) return NULL;

  for (size_t i = 0; i < length; i++) {
      output[j] = malloc(sizeof(char*) * 16); 
      if (output[j] != NULL) {
        sprintf(output[j],"%zu",i);
        j++;
      } else {
        // Handle memory allocation failure
        // Free previously allocated memory before returning NULL
        for (size_t k = 0; k < j; k++) {
          free(output[k]);
        }
        free(output);
        return NULL;
      }
  }
  return output;
}

static JSON_Array_Entry** _$entries(T *self){
  Private *p = self->__private;
  size_t length = p->length;
  JSON_Array_Entry **output = malloc(sizeof(JSON_Array_Entry *) * length);
  if (output == NULL) return NULL;

  for (size_t i=0; i < length; i++) {
    char*key = malloc(sizeof(char*) * 16);
    sprintf(key,"%zu",i);
    JSON_Item item = _$get(self, i); 
    output[i] = malloc(sizeof(JSON_Map_Entry));
    output[i]->key = key;
    output[i]->value = item.value;
    output[i]->type = item.type;
  }
  return output;
}
