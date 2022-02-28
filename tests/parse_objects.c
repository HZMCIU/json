#include "json.h"

#include "unity.h"
#include "unity_internals.h"

void setUp()
{

}

void tearDown()
{

}

void TestJsonParseObject()
{
    Json obj;
    const char* str = "{ \"num\" : 124 , \"lover\" : \"hxy\" , \"array\":[null , false , 123]}";
    TEST_ASSERT_EQUAL(JSON_PARSE_OK, JsonParse(&obj, str));
    Json* item = NULL;
    TEST_ASSERT_EQUAL(JSON_PARSE_OK, JsonGetObjectItem(&obj, "num", &item));
    TEST_ASSERT_EQUAL_INT(124, item->intValue);
    TEST_ASSERT_EQUAL(JSON_PARSE_OK, JsonGetObjectItem(&obj, "lover", &item));
    TEST_ASSERT_EQUAL_STRING("hxy", item->stringValue);
    Json* itemArr = NULL;
    TEST_ASSERT_EQUAL(JSON_PARSE_OK, JsonGetObjectItem(&obj, "array", &itemArr));
    TEST_ASSERT_EQUAL(JSON_ARRAY, itemArr->type);
    TEST_ASSERT_EQUAL(JSON_PARSE_OK, JsonGetArrayItem(itemArr, 0, &item));
    TEST_ASSERT_EQUAL(JSON_NULL, item->type);
    TEST_ASSERT_EQUAL(JSON_PARSE_OK, JsonGetArrayItem(itemArr, 1, &item));
    TEST_ASSERT_EQUAL(JSON_FALSE, item->type);
    TEST_ASSERT_EQUAL(JSON_PARSE_OK, JsonGetArrayItem(itemArr, 2, &item));
    TEST_ASSERT_EQUAL(JSON_NUMBER_INT, item->type);
    TEST_ASSERT_EQUAL_INT(123, item->intValue);
    JsonFree(&obj);
}


int main(int argc, char *argv[])
{
    UNITY_BEGIN();
    RUN_TEST(TestJsonParseObject);
    return UNITY_END();
}
