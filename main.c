#include <json.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char employee[] = "{"
                  "\"firstname\":\"Doe\","
                  "\"lastname\":\"John\","
                  "\"age\":\"20\","
                  "\"children\":[\"Louis\",\"Sam\",\"Antoine\"],"
                  "\"job\":{"
                  "\"title\":\"Administrator system IT\","
                  "\"company\":\"XYZ Corp.\""
                  "}"
                  "}";

char randomValues[] = "["
               "\"Sam\",\"Doe\",\"John\","
               "[\"Louis\",\"Sam\",\"Antoine\"],"
               "[120,43.5,null,true],"
               "\"job\":{"
               "\"title\":\"Administrator system IT\","
               "\"company\":\"XYZ Corp.\""
               "}"
               "]";

void *int_malloc(int value) {
  int *p = malloc(sizeof(int));
  *p = value;
  return p;
}

void print_line(char*msg){
  char* end = "\x1b[0m";
  char*start = "\x1b[32m";
  printf("%s\n* 📚 %s\n%s",start,msg, end);
}

void *double_malloc(double value) {
  double *p = malloc(sizeof(double));
  *p = value;
  return p;
}

void *bool_malloc(bool value) {
  bool *p = malloc(sizeof(bool));
  *p = value;
  return p;
}

int main() {
  // =========================================================================="
  // Parsing
  // =========================================================================="

  JSON_Array *studentsNames = JSON_parse(randomValues).value;
  studentsNames->destructor(studentsNames);

  JSON_Map *employeeMap = JSON_parse(employee).value;
  employeeMap->destructor(employeeMap);

  // =========================================================================="
  // Array usage
  // =========================================================================="

  // 🧰 Creating empty arrays
  // =========================================================================="
  JSON_Array *a = JSON_array_constructor(3);
  JSON_Array *b = JSON_array_constructor(3);

  // 🧰 Pushing some values into an array
  // =========================================================================="
  b->push(b, (JSON_Item){.type = JSON_t_string, .value = strdup("test0")});
  b->push(b, (JSON_Item){.type = JSON_t_string, .value = strdup("test1")});
  a->push(a, (JSON_Item){.type = JSON_t_array, .value = b});

  // 🧰 Retrieve element from an array
  // =========================================================================="
  print_line("Retrieve element from an array");
  JSON_Array *c = a->get(a, 0).value;
  char *name1 = c->get(c, 0).value;
  char *name2 = c->get(c, 1).value;
  printf("name1: %s, name2: %s\n", name1, name2);
  a->destructor(a);

  // 🧰 Iterate over each values of an array
  // =========================================================================="
  print_line("Iterate over each values of an array");
  JSON_Array *d = JSON_array_constructor(3);
  d->push(d, (JSON_Item){.type = JSON_t_string, .value = strdup("test0")});
  d->push(d, (JSON_Item){.type = JSON_t_string, .value = strdup("test1")});
  d->push(d, (JSON_Item){.type = JSON_t_string, .value = strdup("test2")});
  JSON_Item **itemsD = d->values(d);
  for (size_t i = 0; i < d->length(d); i++) {
    printf("item %zu: %s\n", i, (char *)itemsD[i]->value);
    free(itemsD[i]);
  }
  free(itemsD);
  d->destructor(d);

  // 🧰 Iterate over each key/value pairs of an array
  // =========================================================================="
  print_line("Iterate over each key/value pairs of an array");
  JSON_Array *e = JSON_array_constructor(3);
  e->push(e, (JSON_Item){.type = JSON_t_string, .value = strdup("test0")});
  e->push(e, (JSON_Item){.type = JSON_t_string, .value = strdup("test1")});
  e->push(e, (JSON_Item){.type = JSON_t_string, .value = strdup("test2")});
  JSON_Array_Entry **entriesE = e->entries(e);
  for (size_t i = 0; i < e->length(e); i++) {
    char *key = entriesE[i]->key;
    char *value = entriesE[i]->value;
    printf("%s: %s\n", key, value);
    free(key);
    free(entriesE[i]);
  }
  free(entriesE);
  e->destructor(e);

  // 🧰 Iterate over each keys of an array
  // =========================================================================="
  print_line("Iterate over each keys of an array");
  JSON_Array *f = JSON_array_constructor(3);
  f->push(f, (JSON_Item){.type = JSON_t_string, .value = strdup("test0")});
  f->push(f, (JSON_Item){.type = JSON_t_string, .value = strdup("test1")});
  f->push(f, (JSON_Item){.type = JSON_t_string, .value = strdup("test2")});
  char **keysF = f->keys(f);
  for (size_t i = 0; i < f->length(f); i++) {
    printf("key: %s\n", keysF[i]);
    free(keysF[i]);
  }
  free(keysF);
  f->destructor(f);

  // 🧰 Convert an array to a json string
  // =========================================================================="
  print_line("Convert an array to a json string");
  JSON_Array *g = JSON_array_constructor(4);
  JSON_Array *ga = JSON_array_constructor(4);
  JSON_ARRAY_PUSH(ga,.type = JSON_t_string, .value = strdup("test0"));
  JSON_ARRAY_PUSH(ga,.type = JSON_t_string, .value = strdup("test1"));
  JSON_ARRAY_PUSH(g,.type = JSON_t_array, .value=ga);

  JSON_Map *gb = JSON_map_constructor(4);
  JSON_MAP_PUSH(gb,.key="0", .type=JSON_t_string, .value=strdup("0"));
  JSON_MAP_PUSH(gb,.key="1", .type=JSON_t_string, .value=strdup("1"));
  JSON_ARRAY_PUSH(g,.type = JSON_t_map, .value = gb);

  JSON_ARRAY_PUSH(g,.type = JSON_t_string, .value = strdup("test1"));
  JSON_ARRAY_PUSH(g,.type = JSON_t_int, .value = int_malloc(2));
  JSON_ARRAY_PUSH(g,.type = JSON_t_double, .value = double_malloc(2.5));
  JSON_ARRAY_PUSH(g,.type = JSON_t_bool, .value = bool_malloc(false));
  JSON_ARRAY_PUSH(g,.type = JSON_t_null, .value = NULL);
  char *jsonG = g->to_json(g);
  puts(jsonG);
  free(jsonG);
  g->destructor(g);

  // 🧰 Remove an element from an array
  // =========================================================================="
  JSON_Array *h = JSON_array_constructor(4);
  JSON_Array *ha = JSON_array_constructor(4);
  ha->push(ha, (JSON_Item){.type = JSON_t_string, .value = strdup("test0")});
  ha->push(ha, (JSON_Item){.type = JSON_t_string, .value = strdup("test1")});

  JSON_Map *hb = JSON_map_constructor(4);
  JSON_MAP_PUSH(hb,.key="0", .type=JSON_t_string, .value=strdup("0"));
  JSON_MAP_PUSH(hb,.key="1", .type=JSON_t_string, .value=strdup("1"));
  h->push(h, (JSON_Item){.type = JSON_t_map, .value = hb});

  h->push(h, (JSON_Item){.type = JSON_t_array, .value = ha});
  h->push(h, (JSON_Item){.type = JSON_t_string, .value = strdup("test1")});
  h->push(h, (JSON_Item){.type = JSON_t_int, .value = int_malloc(2)});
  h->push(h, (JSON_Item){.type = JSON_t_double, .value = double_malloc(2.5)});
  h->push(h, (JSON_Item){.type = JSON_t_bool, .value = bool_malloc(false)});
  h->push(h, (JSON_Item){.type = JSON_t_null, .value = NULL});
  JSON_ARRAY_PUSH(h,.type=JSON_t_null,.value=NULL);
  h->delete(h,"0");
  h->delete(h,"1");
  h->delete(h,"2");
  h->delete(h,"3");
  h->delete(h,"4");
  h->delete(h,"5");
  h->delete(h,"6");
  h->delete(h,"7");
  h->destructor(h);

  // =========================================================================="
  // map usage
  // =========================================================================="

  // 🧰 Creating empty maps
  // =========================================================================="
  JSON_Map *i = JSON_map_constructor(3);
  JSON_Map *j = JSON_map_constructor(3);

  // 🧰 Pushing some values into an map
  // =========================================================================="
  JSON_MAP_PUSH(j, .type=JSON_t_string, .key="name1", .value=strdup("Jo"));
  JSON_MAP_PUSH(j, .type=JSON_t_string, .key="name2", .value=strdup("Bob"));
  JSON_MAP_PUSH(i, .type=JSON_t_map, .key="names", .value= j);

  // 🧰 Retrieve element from an map
  // =========================================================================="
  print_line("Retrieve element from a map");
  JSON_Map *k = i->get(i, "names").value;
  char *nameK1 = k->get(k, "name1").value;
  char *nameK2 = k->get(k, "name2").value;
  printf("name1: %s, name2: %s\n", nameK1, nameK2);
  i->destructor(i);

  // 🧰 Iterate over each values of a map
  // =========================================================================="
  print_line("Iterate over each values of a map");
  JSON_Map *l = JSON_map_constructor(3);
  JSON_MAP_PUSH(l, .type=JSON_t_string, .key="0", .value=strdup("0"));
  JSON_MAP_PUSH(l, .type=JSON_t_string, .key="1", .value=strdup("1"));
  JSON_MAP_PUSH(l, .type=JSON_t_string, .key="2", .value=strdup("2"));
  JSON_Item **itemsL = l->values(l);
  for (size_t i = 0; i < l->length(l); i++) {
    printf("item %zu: %s\n", i, (char *)itemsL[i]->value);
    free(itemsL[i]);
  }
  free(itemsL);
  l->destructor(l);

  // 🧰 Iterate over each key/value pairs of a map
  // =========================================================================="
  print_line("Iterate over each key/value pairs of a map");
  JSON_Map *m = JSON_map_constructor(3);
  JSON_MAP_PUSH(m, .type=JSON_t_string, .key="0", .value=strdup("0"));
  JSON_MAP_PUSH(m, .type=JSON_t_string, .key="1", .value=strdup("1"));
  JSON_MAP_PUSH(m, .type=JSON_t_string, .key="2", .value=strdup("2"));
  JSON_Map_Entry **entriesM = m->entries(m);
  for (size_t i = 0; i < m->length(m); i++) {
    char *key = entriesM[i]->key;
    char *value = entriesM[i]->value;
    printf("%s: %s\n", key, value);
    free(key);
    free(entriesM[i]);
  }
  free(entriesM);
  m->destructor(m);

  // 🧰 Iterate over each keys of a map
  // =========================================================================="
  print_line("Iterate over each keys of a map");
  JSON_Map *n = JSON_map_constructor(3);
  JSON_MAP_PUSH(n, .type=JSON_t_string, .key="0", .value=strdup("0"));
  JSON_MAP_PUSH(n, .type=JSON_t_string, .key="1", .value=strdup("1"));
  JSON_MAP_PUSH(n, .type=JSON_t_string, .key="2", .value=strdup("2"));
  char **keysN = n->keys(n);
  for (size_t i = 0; i < n->length(n); i++) {
    printf("key: %s\n", keysN[i]);
    free(keysN[i]);
  }
  free(keysN);
  n->destructor(n);

  // 🧰 Remove an element from a map
  // =========================================================================="
  JSON_Map *o = JSON_map_constructor(4);

  JSON_Array *oa = JSON_array_constructor(4);
  JSON_ARRAY_PUSH(oa, .type=JSON_t_string, .value=strdup("0"));
  JSON_ARRAY_PUSH(oa, .type=JSON_t_string, .value=strdup("1"));

  JSON_Map *ob = JSON_map_constructor(4);
  JSON_MAP_PUSH(ob,.key="0", .type=JSON_t_string, .value=strdup("0"));
  JSON_MAP_PUSH(ob,.key="1", .type=JSON_t_string, .value=strdup("1"));

  JSON_MAP_PUSH(o,.key="0", .type=JSON_t_array, .value=oa);
  JSON_MAP_PUSH(o,.key="1", .type=JSON_t_map, .value=ob);
  JSON_MAP_PUSH(o,.key="2", .type=JSON_t_string, .value=strdup("test1"));
  JSON_MAP_PUSH(o,.key="3", .type=JSON_t_int, .value=int_malloc(2));
  JSON_MAP_PUSH(o,.key="4", .type=JSON_t_double, .value=double_malloc(2.5));
  JSON_MAP_PUSH(o,.key="5", .type=JSON_t_bool, .value=bool_malloc(false));
  JSON_MAP_PUSH(o,.key="6", .type=JSON_t_null, .value=NULL);
  o->delete(o,"0");
  o->delete(o,"1");
  o->delete(o,"2");
  o->delete(o,"3");
  o->delete(o,"4");
  o->delete(o,"5");
  o->delete(o,"6");
  o->destructor(o);

  // 🧰 Convert a map to a json string
  // =========================================================================="
  print_line("Convert a map to a json string");
  JSON_Map *p = JSON_map_constructor(4);
  JSON_Array *pa = JSON_array_constructor(4);

  JSON_ARRAY_PUSH(pa,.type=JSON_t_string, .value=strdup("test0"));
  JSON_ARRAY_PUSH(pa,.type=JSON_t_string, .value=strdup("test1"));
  JSON_MAP_PUSH(p,.type=JSON_t_array, .key="0", .value=pa);

  JSON_Map *pb = JSON_map_constructor(4);
  JSON_MAP_PUSH(pb,.key="0", .type=JSON_t_string, .value=strdup("0"));
  JSON_MAP_PUSH(pb,.key="1", .type=JSON_t_string, .value=strdup("1"));
  JSON_MAP_PUSH(p, .key="1", .type=JSON_t_map, .value = pb);

  JSON_MAP_PUSH(p,.key="2",.type = JSON_t_string, .value = strdup("test1"));
  JSON_MAP_PUSH(p,.key="3",.type = JSON_t_int, .value = int_malloc(2));
  JSON_MAP_PUSH(p,.key="4",.type = JSON_t_double, .value = double_malloc(2.5));
  JSON_MAP_PUSH(p,.key="5",.type = JSON_t_bool, .value = bool_malloc(false));
  JSON_MAP_PUSH(p,.key="6",.type = JSON_t_null, .value = NULL);
  char *jsonP = p->to_json(p);
  puts(jsonP);
  free(jsonP);
  p->destructor(p);

  return 0;
}
