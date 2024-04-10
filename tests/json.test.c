#include <criterion/criterion.h>
#include <json.h>

static JSON_Hashmap *h;
static JSON_Array *a;
static char json01[] = "{"
                "\"nom\":\"Doe\","
                "\"prénom\":\"John\","
                "\"age\":\"20\","
                "\"children\":[\"Louis\",\"Sam\",\"Antoine\"],"
                "\"job\":{"
                "\"title\":\"Administrator system IT\","
                "\"company\":\"XYZ Corp.\""
                "}"
                "}";

static char json02[] = "{"
                "\"nom\":\"Doe\","
                "\"prénom\":\"John\","
                "\"age\":20"
                "}";

static char json03[] = "{"
                "\"nom\":\"Doe\","
                "\"prénom\":\"John\","
                "\"age\":20,"
                "\"job\":{"
                "\"title\":\"Administrator system IT\","
                "\"company\":\"XYZ Corp.\""
                "}"
                "}";

static char json04[] = "{"
                "\"nom\":\"Doe\","
                "\"prénom\":\"John\","
                "\"job\":{"
                "\"company\":{"
                "\"name\":\"XYZ Corp.\","
                "\"type\":\"Sa\""
                "}"
                "}"
                "}";

static char json05[] = "{"
                "\"isAdmin\":false,"
                "\"isWorking\":true,"
                "\"company\":null"
                "}";

// static char json06[] = "{"
//                 "\"isAdmin\":\r,"
//                 "\"isWorking\":true,"
//                 "\"company\":null"
//                 "}";

static char json07[] = "{"
                "\"nom\":\"Doe\","
                "\"prénom\":\"John\","
                "\"companies\":["
                "{"
                "\"name\":\"X Corp.\","
                "\"type\":\"Sa\""
                "},"
                "{"
                "\"name\":\"Y Corp.\","
                "\"type\":\"Sa\""
                "},"
                "{"
                "\"name\":\"Z Corp.\","
                "\"type\":\"Sa\""
                "}"
                "]"
                "}";

static char json08[] = "["
                "\"Sam\",\"Doe\",\"John\""
                "]";

static char json09[] = "["
                "{"
                "\"name\":\"X Corp.\","
                "\"type\":\"Sa\""
                "},"
                "{"
                "\"name\":\"Y Corp.\","
                "\"type\":\"Sa\""
                "},"
                "{"
                "\"name\":\"Z Corp.\","
                "\"type\":\"Sa\""
                "}"
                "]";

static char json10[] = "["
                "[\"Sam\",\"Doe\",\"John\"],"
                "[1,2,3],"
                "[true,false,null]"
                "]";

// static void setup(void) {
//   // puts("Runs before the test");
// }

static void array_teardown(void) {
  a->destructor(a);
}
static void map_teardown(void) {
  h->destructor(h);
}

Test(json_to_map, get_string, .fini = map_teardown) {
  JSON_Item item = json_parse(json02);
  h = item.value;
  cr_expect_eq(item.type, JSON_t_map, "Wrong type");
  cr_expect_str_eq(h->get(h, "nom").value, "Doe", "Wrong value");
  cr_expect_str_eq(h->get(h, "prénom").value, "John", "Wrong value");
  cr_expect_eq(*(double*)h->get(h, "age").value, 20, "Wrong value");
  cr_expect_eq(h->length(h), 3, "Wrong size");
}

Test(json_to_map, get_inner_object, .fini = map_teardown) {
  JSON_Item item = json_parse(json03);
  h = item.value;
  cr_expect_eq(item.type, JSON_t_map, "Wrong type");
  JSON_Hashmap *job = h->get(h, "job").value;
  cr_expect_not_null(job, "job is null");
  cr_expect_str_eq(h->get(job, "title").value, "Administrator system IT", "wrong value");
  cr_expect_str_eq(h->get(job, "company").value, "XYZ Corp.", "wrong value");
  cr_expect_eq(h->length(h), 4, "Wrong size");
}

Test(json_to_map, get_inner_inner_object, .fini = map_teardown) {
  JSON_Item item = json_parse(json04);
  h = item.value;
  cr_expect_eq(item.type, JSON_t_map, "Wrong type");
  JSON_Hashmap *job = h->get(h, "job").value;
  cr_expect_not_null(job, "job is null");
  JSON_Hashmap *company = job->get(job, "company").value;
  cr_expect_not_null(company, "company is null");
  cr_expect_str_eq(h->get(company, "name").value, "XYZ Corp.");
  cr_expect_str_eq(h->get(company, "type").value, "Sa");
  cr_expect_eq(h->length(h), 3, "Wrong size");
  cr_expect_eq(company->length(company), 2, "Wrong size");
}

Test(json_to_map, get_primitive, .fini = map_teardown) {
  JSON_Item item = json_parse(json05);
  h = item.value;
  cr_expect_eq(item.type, JSON_t_map, "Wrong type");
  cr_expect_eq(*(bool*)h->get(h, "isAdmin").value, false, "wrong value");
  cr_expect_eq(*(bool*)h->get(h, "isWorking").value, true, "wrong value");
  cr_expect_str_eq(h->get(h, "company").value, "null", "wrong value");
  cr_expect_eq(h->length(h), 3, "Wrong size");
}

Test(json_to_map, get_array, .fini = map_teardown) {
  JSON_Item item = json_parse(json01);
  h = item.value;
  cr_expect_eq(item.type, JSON_t_map, "Wrong type");
  cr_expect_str_eq(h->get(h, "nom").value, "Doe", "Wrong value");
  cr_expect_str_eq(h->get(h, "prénom").value, "John", "Wrong value");
  cr_expect_str_eq(h->get(h, "age").value, "20", "Wrong value");
  cr_expect_eq(h->length(h), 5, "Wrong size");
  JSON_Array *children = h->get(h, "children").value;
  cr_expect_eq(children->length(children), 3, "Wrong size");
  cr_expect_not_null(children, "children is null");
  cr_expect_str_eq(children->get(children,0).value, "Louis", "Wrong value");
  cr_expect_str_eq(children->get(children,1).value, "Sam", "Wrong value");
  cr_expect_str_eq(children->get(children,2).value, "Antoine", "Wrong value");
}

Test(json_to_map, get_array_of_objects, .fini = map_teardown) {
  JSON_Item item = json_parse(json07);
  h = item.value;
  cr_expect_eq(item.type, JSON_t_map, "Wrong type");
  JSON_Array *companies = h->get(h, "companies").value;
  cr_expect_not_null(companies, "companies is null");

  JSON_Hashmap*c1 = companies->get(companies,0).value;
  cr_expect_not_null(c1, "company is null");
  cr_expect_str_eq(c1->get(c1, "name").value, "X Corp.", "Wrong value");
  cr_expect_str_eq(c1->get(c1, "type").value, "Sa", "Wrong value");

  JSON_Hashmap*c2 = companies->get(companies,1).value;
  cr_expect_not_null(c2, "company is null");
  cr_expect_str_eq(c2->get(c2, "name").value, "Y Corp.", "Wrong value");
  cr_expect_str_eq(c2->get(c2, "type").value, "Sa", "Wrong value");

  JSON_Hashmap*c3 = companies->get(companies,2).value;
  cr_expect_not_null(c3, "company is null");
  cr_expect_str_eq(c3->get(c3, "name").value, "Z Corp.", "Wrong value");
  cr_expect_str_eq(c3->get(c3, "type").value, "Sa", "Wrong value");
}

Test(json_to_array, get_array, .fini = array_teardown) {
  JSON_Item item = json_parse(json08);
  a = item.value;
  cr_expect_eq(item.type, JSON_t_array, "Wrong type");
  cr_expect_str_eq(a->get(a,0).value, "Sam",  "Wrong value");
  cr_expect_str_eq(a->get(a,1).value, "Doe",  "Wrong value");
  cr_expect_str_eq(a->get(a,2).value, "John",  "Wrong value");
}

Test(json_to_array, get_array_of_objects, .fini = array_teardown) {
  JSON_Item item = json_parse(json09);
  a = item.value;
  cr_expect_eq(item.type, JSON_t_array, "Wrong type");
  JSON_Hashmap*c1 = a->get(a,0).value;
  cr_expect_not_null(c1, "company is null");
  cr_expect_str_eq(c1->get(c1, "name").value, "X Corp.", "Wrong value");
  cr_expect_str_eq(c1->get(c1, "type").value, "Sa", "Wrong value");
  JSON_Hashmap*c2 = a->get(a,1).value;
  cr_expect_not_null(c2, "company is null");
  cr_expect_str_eq(c2->get(c2, "name").value, "Y Corp.", "Wrong value");
  cr_expect_str_eq(c2->get(c2, "type").value, "Sa", "Wrong value");
  JSON_Hashmap*c3 = a->get(a,2).value;
  cr_expect_not_null(c3, "company is null");
  cr_expect_str_eq(c3->get(c3, "name").value, "Z Corp.", "Wrong value");
  cr_expect_str_eq(c3->get(c3, "type").value, "Sa", "Wrong value");
}

Test(json_to_array, get_array_of_array, .fini = array_teardown) {
  JSON_Item item = json_parse(json10);
  a = item.value;
  cr_expect_eq(item.type, JSON_t_array, "Wrong type");
  JSON_Array*a1 = a->get(a,0).value;
  cr_expect_not_null(a1, "array is null");
  cr_expect_str_eq(a1->get(a1,0).value, "Sam", "Wrong value");
  cr_expect_str_eq(a1->get(a1,1).value, "Doe", "Wrong value");
  cr_expect_str_eq(a1->get(a1,2).value, "John", "Wrong value");
  JSON_Array*a2 = a->get(a,1).value;
  cr_expect_not_null(a2, "array is null");
  cr_expect_str_eq(a2->get(a2,0).value, "1", "Wrong value");
  cr_expect_str_eq(a2->get(a2,1).value, "2", "Wrong value");
  cr_expect_str_eq(a2->get(a2,2).value, "3", "Wrong value");
  JSON_Array*a3 = a->get(a,2).value;
  cr_expect_not_null(a3, "array is null");
  cr_expect_str_eq(a3->get(a3,0).value, "true", "Wrong value");
  cr_expect_str_eq(a3->get(a3,1).value, "false", "Wrong value");
  cr_expect_str_eq(a3->get(a3,2).value, "null", "Wrong value");
}
