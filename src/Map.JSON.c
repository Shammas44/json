#include <pthread.h>
#include <logger/logger.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Map.JSON.h"
#include "Array.JSON.h"
#define T JSON_Map
#define MAX_CAPACITY 10000
#define LOAD_FACTOR_THRESHOLD 0.7

typedef struct {
  char *key;
  void *value;
  JSON_t type;
  bool is_destroyable;
} Entry;

typedef struct {
  Entry *entries;
  size_t capacity;
  size_t length;
  pthread_mutex_t mutex;
} Private;

static size_t __capacity(T *self);
static size_t __length(T *self);
static JSON_Item __get(T *self, char*key);
static void __push(T *self, JSON_Map_Entry entry);
static int __delete(T *self, char* key);
static char *__to_json(T *self);
static char **__keys(T *self);
static JSON_Item **__values(T *self);
static JSON_Map_Entry **__entries(T *self);
void __destructor(T *self);

static char *_$to_json(T *self);
static int _$compareStrings(const void *a, const void *b);
static char* _$convertToTrimmedString(double num);
static void _$free_entry(Entry entry);
static void _$resize(T *self);
static unsigned int __$hash(const char *key, size_t capacity);
static char **_$keys(T *self);
static JSON_Item **_$values(T *self);
static JSON_Map_Entry **_$entries(T *self);
static JSON_Item _$get(T *self, char*key);

static JSON_IsDestroyable d = {
  .destructor = __destructor
};


T *JSON_map_constructor(size_t initial_capacity) {
  if (initial_capacity > MAX_CAPACITY)
    return NULL;
  if (initial_capacity < 1)
    return NULL;

  T *self = (T *)malloc(sizeof(T));
  if (self == NULL) {
    LOG_ERROR("map is NULL");
    return NULL;
  }
  Private *private = (Private *)malloc(sizeof(Private));
  self->__private = private;
  self->length = __length;
  self->capacity = __capacity;
  self->destructor = __destructor;
  self->get = __get;
  self->delete = __delete;
  self->push = __push;
  self->to_json = __to_json;
  self->keys = __keys;
  self->values = __values;
  self->entries = __entries;
  self->__destructor = d;
  pthread_mutex_init(&private->mutex, NULL);
  initial_capacity =
      initial_capacity + (initial_capacity * LOAD_FACTOR_THRESHOLD);
  if (initial_capacity > MAX_CAPACITY)
    initial_capacity = MAX_CAPACITY;

  if (self != NULL) {
    private->entries = (Entry *)malloc(sizeof(Entry) * initial_capacity);
    if (private->entries != NULL) {
      memset(private->entries, 0, sizeof(Entry) * initial_capacity);
      private->capacity = initial_capacity;
      private->length = 0;
      return self;
    }
    free(self);
  }
  return NULL;
}

void __destructor(T *self) {
  if (self != NULL) {
    Private *private = self->__private;
    pthread_mutex_lock(&private->mutex);
    Entry *entries = private->entries;

    for (size_t i = 0; i < private->capacity; i++) {
      if (entries[i].key == NULL) {
        continue;
      }
      _$free_entry(entries[i]);
    }
    free(entries);
    pthread_mutex_unlock(&private->mutex);
    pthread_mutex_destroy(&private->mutex);
    free(private);
    free(self);
  }
}

static size_t __length(T *self) {
  Private *p = self->__private;
  pthread_mutex_lock(&p->mutex);
  size_t length = p->length;
  pthread_mutex_unlock(&p->mutex);
  return length;
}

static size_t __capacity(T *self) {
  Private *p = self->__private;
  pthread_mutex_lock(&p->mutex);
  size_t capacity = p->capacity;
  pthread_mutex_unlock(&p->mutex);
  return capacity;
}

static char **__keys(T *self) {
  Private *p = self->__private;
  pthread_mutex_lock(&p->mutex);
  char **keys = _$keys(self);
  pthread_mutex_unlock(&p->mutex);
  return keys;
}

static JSON_Item **__values(T *self) {
  Private *p = self->__private;
  pthread_mutex_lock(&p->mutex);
  JSON_Item **values = _$values(self);
  pthread_mutex_unlock(&p->mutex);
  return values;
}

static JSON_Map_Entry **__entries(T *self){
  Private *p = self->__private;
  pthread_mutex_lock(&p->mutex);
  JSON_Map_Entry **entries = _$entries(self);
  pthread_mutex_unlock(&p->mutex);
  return entries;
}

static JSON_Item __get(T *self, char*key) {
  Private *private = self->__private;
  pthread_mutex_lock(&private->mutex);
  JSON_Item item = _$get(self, key);
  pthread_mutex_unlock(&private->mutex);
  return item;
}

static int __delete(T *self, char* key){
  Private *private = self->__private;
  int status = 0;
  pthread_mutex_lock(&private->mutex);
  Entry *entries = private->entries;
  unsigned int index = __$hash(key, private->capacity);
  Entry entry;
  while (entries[index].key != NULL) {
    if (strcmp(entries[index].key, key) == 0) {
      entry = entries[index];
      _$free_entry(entry);
      entries[index].key = NULL;
      private->length = private->length - 1;
      break;
    }
    // Linear probing for collision resolution
    index = (index + 1) % private->capacity;
  }
  status = 1; 
  pthread_mutex_unlock(&private->mutex);
  return status;
}

static void __push(T *self, JSON_Map_Entry entry) {
  Private *private = self->__private;
  pthread_mutex_lock(&private->mutex);
  size_t *capacity = &private->capacity;
  size_t *length = &private->length;
  Entry *entries = private->entries;
  JSON_t type = entry.type;
  void *value = entry.value;
  const char*key = entry.key;

  unsigned int index = __$hash(key, *capacity);

  bool isReplacement = false;
  if (entries[index % *capacity].key != NULL) {
    if (strcmp(entries[index % *capacity].key, key) == 0) {
      isReplacement = true;
      index = index % *capacity;
      _$free_entry(entries[index % *capacity]);
    } else {
      while (entries[index].key != NULL) {
        index = (index + 1) % *capacity;
        if (entries[index % *capacity].key != NULL &&
            strcmp(entries[index].key, key) == 0) {
          isReplacement = true;
          break;
        }
      }
    }
  }

  if (!isReplacement) {
    while (entries[index].key != NULL) {
      index = (index + 1) % *capacity;
    }
  }

  entries[index].key = strdup(key);
  entries[index].value = value;
  entries[index].type = type;
  entries[index].is_destroyable = false;
  if(type == JSON_t_map || type == JSON_t_array) {
  entries[index].is_destroyable = true;
  }

  if (!isReplacement) {
    *length += 1;
    if ((double)(*length) / *capacity > LOAD_FACTOR_THRESHOLD) {
      _$resize(self);
    }
  }
  pthread_mutex_unlock(&private->mutex);
}

static char* __to_json(T* self) {
  Private *private = self->__private;
  pthread_mutex_lock(&private->mutex);
  char* json = _$to_json(self);
  pthread_mutex_unlock(&private->mutex);
  return json;
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

  strcpy(json, "{");

  Private* p = self->__private;
  size_t length = p->length;
  JSON_Map_Entry** entries = _$entries(self);

  for (size_t i = 0; i < length; i++) {
    char* key = entries[i]->key;
    char* value = NULL;
    char* tick = "\"";
    char* comma = (i == length - 1) ? "" : ",";
    JSON_Array*array;

    switch (entries[i]->type) {
      case JSON_t_string:
      case JSON_t_default:
        value = entries[i]->value;
        break;

      case JSON_t_double:
        value = _$convertToTrimmedString(*(double*)entries[i]->value);
        tick = "";
        break;

      case JSON_t_int:
        value = malloc(sizeof(char) * 100);
        if (value != NULL) {
          sprintf(value, "%d", *(int*)entries[i]->value);
          tick = "";
        } else {
          free(json);
          return NULL;  // Memory allocation failure
        }
        break;

      case JSON_t_bool:
        value = *(bool*)entries[i]->value ? "true" : "false";
        tick = "";
        break;

      case JSON_t_null:
        value = "null";
        tick = "";
        break;

      case JSON_t_map:
        value = _$to_json(entries[i]->value);
        tick = "";
        break;
      case JSON_t_array:
        array = entries[i]->value;
        value = array->to_json(array);
        tick = "";
        break;

      default:
        // Handle unsupported type or error
        free(json);
        return NULL;
    }

    strcat(json, "\"");
    strcat(json, key);
    strcat(json, "\":");
    strcat(json, tick);
    strcat(json, value);
    strcat(json, tick);
    strcat(json, comma);

    // Free dynamically allocated memory for value (if any)
    JSON_t to_free[4] = {JSON_t_int,JSON_t_double,JSON_t_array,JSON_t_map};
    for (int j=0; j<4; j++) {
    if (entries[i]->type == to_free[j]) {
        free(value);
        break;
      }
    }
    free(key);
    free(entries[i]);
  }

  strcat(json, "}");
  free(entries);
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

static unsigned int __$hash(const char *key, size_t capacity) {
  unsigned int hash = 0;
  for (int i = 0; key[i] != '\0'; i++) {
    hash = 31 * hash + key[i];
  }
  return hash % capacity;
}

static void _$resize(T *self) {
  Private *private = self->__private;
  int new_capacity = private->capacity * 2;
  Entry *entries = private->entries;
  Entry *new_entries = (Entry *)malloc(sizeof(Entry) * new_capacity);

  if (new_entries != NULL) {
    // Initialize new_entries to NULL key
    for (int i = 0; i < new_capacity; i++) {
      new_entries[i].key = NULL;
    }

    // Rehash and move elements to the new array
    for (size_t i = 0; i < private->capacity; i++) {
      if (entries[i].key != NULL) {
        unsigned int index = __$hash(entries[i].key, new_capacity);
        // Linear probing for collision resolution
        while (new_entries[index].key != NULL) {
          index = (index + 1) % new_capacity;
        }
        // Copy the entry to the new array
        new_entries[index] = entries[i];
      }
    }

    // Free the old entries and update map properties
    free(entries);
    private->entries = new_entries;
    private->capacity = new_capacity;
  }
}

static int _$compareStrings(const void *a, const void *b) {
    return strcmp(*(const char**)a, *(const char**)b);
}

static void _$free_entry(Entry entry){
  bool is_destroyable = entry.is_destroyable;
  void *value = entry.value;
  free(entry.key);
  if(value ==NULL) return;
  if (is_destroyable) {
    JSON_IsDestroyable *element = (JSON_IsDestroyable *)value;
    void (*destructor)(void *) = element->destructor;
    destructor(value);
  } else {
    free(value);
  }
}

static char **_$keys(T *self) {
  Private *p = self->__private;
  Entry *entries = p->entries;
  size_t length = p->length;
  size_t capacity = p->capacity;
  char **output = malloc(sizeof(char *) * length);
  size_t j = 0;
  if (output == NULL){
    return NULL;
  }

  for (size_t i = 0; i < capacity; i++) {
    if (entries[i].key != NULL) {
      output[j] = malloc(strlen(entries[i].key) + 1);

      if (output[j] != NULL) {
        strcpy(output[j], entries[i].key);
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
  }

  qsort(output, length, sizeof(char*), _$compareStrings);
  return output;
}

static JSON_Item _$get(T *self, char*key) {
  Private *private = self->__private;
  Entry *entries = private->entries;
  unsigned int index = __$hash(key, private->capacity);
  Entry entry;
  while (entries[index].key != NULL) {
    if (strcmp(entries[index].key, key) == 0) {
      entry = entries[index];
      return (JSON_Item){.type = entry.type, .value = entry.value};
    }
    // Linear probing for collision resolution
    index = (index + 1) % private->capacity;
  }
  return (JSON_Item){.type = JSON_t_null, .value = NULL};
}

static JSON_Item **_$values(T *self) {
  Private *p = self->__private;
  size_t length = p->length;
  if(length == 0){
    return NULL;
  } 
  JSON_Item **output = malloc(sizeof(JSON_Item *) * length);
  if (output == NULL){
    return NULL;
  } 
  char**keys = _$keys(self);
  for (size_t i=0; i < length; i++) {
    JSON_Item item = _$get(self, keys[i]); 
    output[i] = malloc(sizeof(JSON_Item));
    output[i]->value = item.value;
    output[i]->type = item.type;
    free(keys[i]);
  }
  free(keys);
  return output;
}

static JSON_Map_Entry **_$entries(T *self){
  Private *p = self->__private;
  size_t length = p->length;
  if(length == 0){
    return NULL;
  } 
  JSON_Map_Entry **output = malloc(sizeof(JSON_Map_Entry *) * length);
  if (output == NULL){
    return NULL;
  } 
  char**keys = _$keys(self);
  for (size_t i=0; i < length; i++) {
    char*key = keys[i];
    JSON_Item item = _$get(self, key); 
    output[i] = malloc(sizeof(JSON_Map_Entry));
    output[i]->key = key;
    output[i]->value = item.value;
    output[i]->type = item.type;
  }
  free(keys);
  return output;
}

