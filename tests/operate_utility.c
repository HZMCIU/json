#include "json.h"

#include <stdlib.h>

#include "unity.h"
#include "unity_internals.h"

void setUp()
{

}

void tearDown()
{

}

void TestAddItemToArray()
{
    Json* array = JsonCreateArray();
    TEST_ASSERT_EQUAL_INT(JSON_ADD_OK, JsonAddItemToArray(array, JsonCreateNull()));
    TEST_ASSERT_EQUAL_INT(JSON_ADD_OK, JsonAddItemToArray(array, JsonCreateFalse()));
    TEST_ASSERT_EQUAL_INT(JSON_ADD_OK, JsonAddItemToArray(array, JsonCreateTrue()));
    TEST_ASSERT_EQUAL_INT(JSON_ADD_OK, JsonAddItemToArray(array, JsonCreateNumber(3.1415)));
    TEST_ASSERT_EQUAL_INT(JSON_ADD_OK, JsonAddItemToArray(array, JsonCreateString("hello")));

    Json* item = NULL;
    JsonGetArrayItem(array, 0, &item);
    TEST_ASSERT_EQUAL_INT(JSON_STRING, item->type);
    TEST_ASSERT_EQUAL_STRING("hello", item->stringValue);
    JsonGetArrayItem(array, 1, &item);
    TEST_ASSERT_EQUAL_INT(JSON_NUMBER_FLOAT, item->type);
    TEST_ASSERT_EQUAL_DOUBLE(3.1415, item->doubleValue);
    JsonGetArrayItem(array, 2, &item);
    TEST_ASSERT_EQUAL_INT(JSON_TRUE, item->type);
    JsonGetArrayItem(array, 3, &item);
    TEST_ASSERT_EQUAL_INT(JSON_FALSE, item->type);
    JsonGetArrayItem(array, 4, &item);
    TEST_ASSERT_EQUAL_INT(JSON_NULL, item->type);

    JsonFree(array);
    free(array);
}

void TestAddItemToObject()
{
    Json* obj = JsonCreateObject();
    TEST_ASSERT_EQUAL_INT(JSON_ADD_OK, JsonAddItemToObject(obj, "null", JsonCreateNull()));
    TEST_ASSERT_EQUAL_INT(JSON_ADD_OK, JsonAddItemToObject(obj, "false", JsonCreateFalse()));
    TEST_ASSERT_EQUAL_INT(JSON_ADD_OK, JsonAddItemToObject(obj, "true", JsonCreateTrue()));
    TEST_ASSERT_EQUAL_INT(JSON_ADD_OK, JsonAddItemToObject(obj, "number", JsonCreateNumber(3.1415)));
    TEST_ASSERT_EQUAL_INT(JSON_ADD_OK, JsonAddItemToObject(obj, "string", JsonCreateString("hello")));

    Json *item = NULL;
    JsonGetObjectItem(obj, "null", &item);
    TEST_ASSERT_EQUAL_INT(JSON_NULL, item->type);
    JsonGetObjectItem(obj, "false", &item);
    TEST_ASSERT_EQUAL_INT(JSON_FALSE, item->type);
    JsonGetObjectItem(obj, "true", &item);
    TEST_ASSERT_EQUAL_INT(JSON_TRUE, item->type);
    JsonGetObjectItem(obj, "number", &item);
    TEST_ASSERT_EQUAL_INT(JSON_NUMBER_FLOAT, item->type);
    TEST_ASSERT_EQUAL_DOUBLE(3.1415, item->doubleValue);
    JsonGetObjectItem(obj, "string", &item);
    TEST_ASSERT_EQUAL_INT(JSON_STRING, item->type);
    TEST_ASSERT_EQUAL_STRING("hello", item->stringValue);
    JsonFree(obj);
    free(obj);
}

int main(int argc, char *argv[])
{
    UNITY_BEGIN();
    RUN_TEST(TestAddItemToArray);
    RUN_TEST(TestAddItemToObject);
    return UNITY_END();
}
