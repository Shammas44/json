#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <logger.h>
#include "json.h"

// =========================================================================="
// Prototypes functions
// =========================================================================="

static int _$json_parse(char *json, jsmntok_t **tokens);
static JSON_Hashmap* _$json_to(char *json, jsmntok_t *tokens, int token_num);
static void *_$extract_string(char*json,jsmntok_t*token);

// =========================================================================="
// Public functions
// =========================================================================="

int json_to_map(char *json, JSON_Hashmap**map,jsmntok_t *tokens, int token_num) {
  if(tokens ==NULL && token_num == 0){
   tokens = NULL;
   token_num = _$json_parse(json, &tokens);
  }
  if(tokens[0].type != JSMN_OBJECT) return 0;
  if (token_num <= 0) return -1;
  *map = _$json_to(json, tokens, token_num);
  return token_num;
}

int json_to_array(char *json, JSON_Array**array,jsmntok_t *tokens, int token_num){
  void* value = NULL;
  if(tokens ==NULL && token_num == 0){
   tokens = NULL;
   token_num = _$json_parse(json, &tokens);
  }
  if(tokens[0].type != JSMN_ARRAY) return 0;
  if (token_num <= 0) return 0;
  *array = JSON_array_constructor(token_num);
  if(array == NULL) return 0;
  for (int i = 1; i < token_num; i++) {
    int inner_token_num = 0;
    char*string = _$extract_string(json,&tokens[i]);

    if(string == NULL) return 0;

    if(tokens[i].type == JSMN_OBJECT){
      inner_token_num = json_to_map(string, (JSON_Hashmap**)&value,NULL,0);
      inner_token_num--;
    }
    else if(tokens[i].type == JSMN_ARRAY){
      inner_token_num = json_to_array(string, (JSON_Array**)&value,NULL,0);
      inner_token_num--;
    }else {
      value = string;
    }
    (*array)->push(*array,(JSON_Item){.type=JSON_t_string,.value=value});
    i+=inner_token_num;
  }
  return token_num;
}

void* json_parse(char*input){
  jsmntok_t *tokens = NULL;
  int token_num = _$json_parse(input, &tokens);
  void*value =NULL;
  if(token_num <= 1) return NULL;
  jsmntype_t type = tokens[0].type;
  switch (type) {
    case JSMN_OBJECT:
      json_to_map(input, (JSON_Hashmap**)&value,NULL,0);
    break;
    case JSMN_ARRAY:
      json_to_array(input, (JSON_Array**)&value,NULL,0);
    break;
    case JSMN_STRING:
      value = NULL;
    break;
    case JSMN_PRIMITIVE:
      value = NULL;
    break;
    case JSMN_UNDEFINED:
      value = NULL;
    break;
  }
  return value;
}

// =========================================================================="
// Private functions
// =========================================================================="

static int _$json_parse(char *json, jsmntok_t **tokens) {
  int token_num = 0;
  jsmn_parser parser;
  jsmn_init(&parser);

  int required_tokens = jsmn_parse(&parser, json, strlen(json), NULL, 0);

  *tokens = malloc(sizeof(jsmntok_t) * required_tokens);
  if (!*tokens) {
    LOG_ERROR("Memory allocation failed");
    return 1;
  }

  jsmn_init(&parser);
  token_num = jsmn_parse(&parser, json, strlen(json), *tokens, required_tokens);

  if (token_num < 0) {
    free(*tokens);
    *tokens = NULL;
    LOG_ERROR("Failed to parse JSON");
    return 1;
  }

  if (token_num < 1 ) {
    free(*tokens);
    *tokens = NULL;
    LOG_ERROR("Nothing to parse");
    return 1;
  }

  return token_num;
}

static void *_$extract_string(char*json,jsmntok_t*token){
      int token_start = token->start;
      int token_end = token->end;
      int token_length = token_end - token_start;
      if (token_length <= 0)  return NULL; 
      char *token_string = json + token_start;
      char *string = malloc(token_length + 1);
      if (string == NULL)  return NULL; 
      strncpy(string, token_string, token_length);
      string[token_length] = '\0';
      return string;
}

static JSON_Hashmap* _$json_to(char *json, jsmntok_t *tokens, int token_num) {
  JSON_Hashmap *map = hashmap_constructor(100);
  JSON_Hashmap*inner_map = NULL; 
  JSON_Array*inner_array = NULL; 
  if (map == NULL) return NULL;

  for (int i = 1; i < token_num; i++) {
    int inner_token_num = 0;
    if (tokens[i].type == JSMN_STRING && tokens[i + 1].type != JSMN_UNDEFINED) {
      // If the current token is a string and the next token exists
      // Then it's a key-value pair
      char*key = _$extract_string(json,&tokens[i]);
      i++;
      char *value = _$extract_string(json,&tokens[i]);
      // printf("%s: %s\n", key, value);

      void *primitive;
      JSON_Hashmap_Entry primitive_entry = (JSON_Hashmap_Entry){.key=key};
      if(tokens[i].type == JSMN_PRIMITIVE){
        if(strcasecmp(value, "true") == 0 || strcasecmp(value, "false") == 0){
          primitive = malloc(sizeof(bool));
          *(bool*)primitive = strcasecmp(value, "true") == 0 ? true : false;
          primitive_entry.type = JSON_t_bool;
          primitive_entry.value = primitive;
        }else if(strcasecmp(value, "null") == 0){
          primitive = malloc(sizeof(char*) *5);
          sprintf(primitive,"%s",value);
          primitive_entry.type = JSON_t_null;
          primitive_entry.value = primitive;
        }else {
          primitive = malloc(sizeof(double));
          *(double*) primitive = atof(value);
          primitive_entry.type = JSON_t_double;
          primitive_entry.value = primitive;
        }
        free(value);
      }

      switch (tokens[i].type) {
        case JSMN_STRING:
          map->push(map,(JSON_Hashmap_Entry){.key=key,.type=JSON_t_string,.value=value});
          break;
        case JSMN_PRIMITIVE:
          map->push(map,primitive_entry);
          break;
        case JSMN_OBJECT:
          inner_token_num = json_to_map(value, &inner_map,NULL,0);
          inner_token_num--;
          map->push(map,(JSON_Hashmap_Entry){.key=key,.type=JSON_t_map,.value=inner_map});
          break;
        case JSMN_ARRAY:
          inner_token_num = json_to_array(value,&inner_array,NULL,0);
          inner_token_num--;
          map->push(map,(JSON_Hashmap_Entry){.key=key,.type=JSON_t_array,.value=inner_array});
          break;
        default:
          LOG_ERROR("Unhandled type");
          break;
      }
      i+=inner_token_num;
    }
  }
  return map;
}

