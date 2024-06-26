#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <logger.h>
#include "jsmn.JSON.h"
#include "Map.JSON.h"
#include "parser.JSON.h"

// =========================================================================="
// Prototypes functions
// =========================================================================="

static int _$json_parse(char *json, jsmntok_t **tokens);
static JSON_Map* _$json_to(char *json, jsmntok_t *tokens, int token_num);
static void *_$extract_string(char*json,jsmntok_t*token);
static int _$to_map(char *json, JSON_Map**map,jsmntok_t *tokens, int token_num);
static int _$to_array(char *json, JSON_Array**array,jsmntok_t *tokens, int token_num);

// =========================================================================="
// Public functions
// =========================================================================="

JSON_Item JSON_parse(char*input){
  jsmntok_t *tokens = NULL;
  int token_num = _$json_parse(input, &tokens);
  void*value =NULL;
  JSON_t output_t = JSON_t_null;
  if(token_num <= 1) return (JSON_Item){.type=output_t,.value=NULL};
  jsmntype_t type = tokens[0].type;
  switch (type) {
    case JSMN_OBJECT:
      _$to_map(input, (JSON_Map**)&value,NULL,0);
      output_t = JSON_t_map;
    break;
    case JSMN_ARRAY:
      _$to_array(input, (JSON_Array**)&value,NULL,0);
      output_t = JSON_t_array;
    break;
    case JSMN_STRING:
      value = NULL;
      output_t = JSON_t_string;
    break;
    case JSMN_PRIMITIVE:
      value = NULL;
      output_t = JSON_t_double;
    break;
    case JSMN_UNDEFINED:
      value = NULL;
      output_t = JSON_t_string;
    break;
  }
  free(tokens);
  return (JSON_Item){.type=output_t,.value=value};
}

// =========================================================================="
// Private functions
// =========================================================================="

static int _$to_map(char *json, JSON_Map**map,jsmntok_t *tokens, int token_num) {
  if(tokens ==NULL && token_num == 0){
   tokens = NULL;
   token_num = _$json_parse(json, &tokens);
  }
  if (token_num <= 0) return -1;
  if(tokens[0].type != JSMN_OBJECT){
    free(tokens);
    return -1;
  } 
  *map = _$json_to(json, tokens, token_num);
  free(tokens);
  return token_num;
}

static int _$to_array(char *json, JSON_Array**array,jsmntok_t *tokens, int token_num){
  void* value = NULL;
  JSON_t type;
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
      inner_token_num = _$to_map(string, (JSON_Map**)&value,NULL,0);
      inner_token_num--;
      type = JSON_t_map;
      free(string);
    }
    else if(tokens[i].type == JSMN_ARRAY){
      inner_token_num = _$to_array(string, (JSON_Array**)&value,NULL,0);
      inner_token_num--;
      type = JSON_t_array;
      free(string);
    }else if(strcmp(string, "null")==0){
      value = NULL;
      type = JSON_t_null;
      free(string);
    }else {
      value = string;
      type = JSON_t_string;
    }
    (*array)->push(*array,(JSON_Item){.type=type,.value=value});
    i+=inner_token_num;
  }
  
  free(tokens);
  return token_num;
}

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

static JSON_Map* _$json_to(char *json, jsmntok_t *tokens, int token_num) {
  JSON_Map *map = JSON_map_constructor(100);
  JSON_Map*inner_map = NULL; 
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
      JSON_Map_Entry primitive_entry = (JSON_Map_Entry){.key=key};
      if(tokens[i].type == JSMN_PRIMITIVE){
        if(strcasecmp(value, "true") == 0 || strcasecmp(value, "false") == 0){
          primitive = malloc(sizeof(bool));
          *(bool*)primitive = strcasecmp(value, "true") == 0 ? true : false;
          primitive_entry.type = JSON_t_bool;
          primitive_entry.value = primitive;
        }else if(strcasecmp(value, "null") == 0){
          primitive_entry.type = JSON_t_null;
          primitive_entry.value = NULL;
        }else {
          primitive = malloc(sizeof(double));
          *(double*) primitive = atof(value);
          primitive_entry.type = JSON_t_double;
          primitive_entry.value = primitive;
        }
        free(value);
      }

      if(value!=NULL){
        switch (tokens[i].type) {
          case JSMN_STRING:
            map->push(map,(JSON_Map_Entry){.key=key,.type=JSON_t_string,.value=value});
            break;
          case JSMN_PRIMITIVE:
            map->push(map,primitive_entry);
            break;
          case JSMN_OBJECT:
            inner_token_num = _$to_map(value, &inner_map,NULL,0);
            inner_token_num--;
            map->push(map,(JSON_Map_Entry){.key=key,.type=JSON_t_map,.value=inner_map});
            free(value);
            break;
          case JSMN_ARRAY:
            inner_token_num = _$to_array(value,&inner_array,NULL,0);
            inner_token_num--;
            map->push(map,(JSON_Map_Entry){.key=key,.type=JSON_t_array,.value=inner_array});
            free(value);
            break;
          default:
            LOG_ERROR("Unhandled type");
            break;
        }
      }else{
        map->push(map,(JSON_Map_Entry){.key=key,.type=JSON_t_null,.value=NULL});
      }
      free(key);
      i+=inner_token_num;
    }
  }
  return map;
}

