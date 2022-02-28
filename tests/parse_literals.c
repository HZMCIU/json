#include "json.h"

#include <stdio.h>

#include "unity.h"

void setUp()
{

}

void tearDown()
{

}

void TestParseJsonNull()
{
    Json item;
    TEST_ASSERT_EQUAL_MESSAGE(JSON_PARSE_OK, JsonParse(&item, "null"), "valid null");
    TEST_ASSERT_EQUAL(JSON_NULL, item.type);

    TEST_ASSERT_EQUAL_MESSAGE(JSON_INVALID_VALUE, JsonParse(&item, "nulx"), "invalid null");
    TEST_ASSERT_EQUAL_MESSAGE(JSON_INVALID_VALUE, JsonParse(&item, "nul"), "invalid null");
}

void TestParseJsonBoolean()
{
    Json item;

    /*valid boolean value testing*/
    TEST_ASSERT_EQUAL_MESSAGE(JSON_PARSE_OK, JsonParse(&item, "false"), "valid boolean(false)");
    TEST_ASSERT_EQUAL(JSON_FALSE, item.type);

    TEST_ASSERT_EQUAL_MESSAGE(JSON_PARSE_OK, JsonParse(&item, "true"), "valid boolean(true)");
    TEST_ASSERT_EQUAL(JSON_TRUE, item.type);

    /*invalid boolean value testing.*/
    TEST_ASSERT_EQUAL_MESSAGE(JSON_INVALID_VALUE, JsonParse(&item, "trux"), "invalid boolean");
    TEST_ASSERT_EQUAL_MESSAGE(JSON_INVALID_VALUE, JsonParse(&item, "falsx"), "invalid boolean");
    TEST_ASSERT_EQUAL_MESSAGE(JSON_INVALID_VALUE, JsonParse(&item, "tru"), "invalid boolean");
}
int main(int argc, char *argv[])
{
    UNITY_BEGIN();
    RUN_TEST(TestParseJsonNull);
    RUN_TEST(TestParseJsonBoolean);
    return UNITY_END();
}
