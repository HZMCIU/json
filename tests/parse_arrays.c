#include "json.h"

#include "unity.h"
#include "unity_internals.h"

void setUp()
{

}

void tearDown()
{

}

void TestJsonParseArray()
{
    Json arr;
    JsonInit(&arr);
    TEST_ASSERT_EQUAL(JSON_PARSE_OK, JsonParse(&arr, "[0,1,2,3]"));
    TEST_ASSERT_EQUAL(JSON_ARRAY, arr.type);
    for(int i = 0; i < 4; i++) {
        Json* item = NULL;
        TEST_ASSERT_EQUAL(JSON_PARSE_OK, JsonGetArrayItem(&arr, i, &item));
        TEST_ASSERT_EQUAL(JSON_NUMBER_INT, item->type);
        TEST_ASSERT_EQUAL(i, item->intValue);
    }
    JsonFree(&arr);

    JsonInit(&arr);
    TEST_ASSERT_EQUAL(JSON_PARSE_OK, JsonParse(&arr, "[ null , false , true , 123 , 3.1415e92 , \"abc\" ]"));
    TEST_ASSERT_EQUAL(6, JsonGetArraySize(&arr));
    TEST_ASSERT_EQUAL(JSON_ARRAY, arr.type);
    Json* item = NULL;
    JsonGetArrayItem(&arr, 0, &item);
    TEST_ASSERT_EQUAL(JSON_NULL, item->type);
    JsonGetArrayItem(&arr, 1, &item);
    TEST_ASSERT_EQUAL(JSON_FALSE, item->type);
    JsonGetArrayItem(&arr, 2, &item);
    TEST_ASSERT_EQUAL(JSON_TRUE, item->type);
    JsonGetArrayItem(&arr, 3, &item);
    TEST_ASSERT_EQUAL(JSON_NUMBER_INT, item->type);
    TEST_ASSERT_EQUAL(123, item->intValue);
    JsonGetArrayItem(&arr, 4, &item);
    TEST_ASSERT_EQUAL(JSON_NUMBER_FLOAT, item->type);
    TEST_ASSERT_EQUAL_DOUBLE(3.1415e92, item->doubleValue);
    JsonGetArrayItem(&arr, 5, &item);
    TEST_ASSERT_EQUAL(JSON_STRING, item->type);
    TEST_ASSERT_EQUAL_STRING("abc", item->stringValue);
    JsonFree(&arr);

    JsonInit(&arr);
    TEST_ASSERT_EQUAL(JSON_PARSE_OK, JsonParse(&arr, "[ [ ] , [ 0 ] , [ 0 , 1 ] , [ 0 , 1 , 2 ] ]"));
    TEST_ASSERT_EQUAL(JSON_ARRAY, arr.type);
    for(size_t i = 0; i < 4; i++) {
        JsonGetArrayItem(&arr, i, &item);
        TEST_ASSERT_EQUAL(JSON_ARRAY, item->type);
        TEST_ASSERT_EQUAL(i, JsonGetArraySize(item));
        if (i > 0) {
            for(size_t j = 0; j < i; j++) {
                Json* iitem = NULL;
                JsonGetArrayItem(item, j, &iitem);
                TEST_ASSERT_EQUAL(JSON_NUMBER_INT, iitem->type);
                TEST_ASSERT_EQUAL(j, iitem->intValue);
            }
        }
    }
    JsonFree(&arr);
}

void TestParseInvalidArray()
{
    Json item;
    JsonInit(&item);
    TEST_ASSERT_EQUAL(JSON_INVALID_ARRAY, JsonParse(&item, "[1,2,3"));
    TEST_ASSERT_EQUAL(JSON_INVALID_VALUE, JsonParse(&item, "[1,2,3,]"));
}

int main(int argc, char *argv[])
{
    UNITY_BEGIN();
    RUN_TEST(TestJsonParseArray);
    RUN_TEST(TestParseInvalidArray);
    return UNITY_END();
}
