#include "json.h"

#include "unity.h"
#include "unity_internals.h"

void setUp()
{

}

void tearDown()
{

}

void TestJsonParseString()
{
    Json item;
    JsonInit(&item);
    const char* str = "\"hello world\"";
    TEST_ASSERT_EQUAL_MESSAGE(JSON_PARSE_OK, JsonParse(&item, str), "valid string \"hello world\"");
    TEST_ASSERT_EQUAL(JSON_STRING, item.type);
    TEST_ASSERT_EQUAL_STRING("hello world", item.stringValue);
    JsonFree(&item);


    str = "\"\\\" \\\\ \\/ \\b \\f \\n \\r \\t\"";
    JsonInit(&item);
    TEST_ASSERT_EQUAL_MESSAGE(JSON_PARSE_OK, JsonParse(&item, str), "escape characters \" \\\" \\\\ \\/ \\b \\f \\n \\r \\t\"");
    TEST_ASSERT_EQUAL(JSON_STRING, item.type);
    TEST_ASSERT_EQUAL_STRING("\" \\ / \b \f \n \r \t", item.stringValue);
    JsonFree(&item);
}

void TestJsonParseStringUtf()
{
    Json item;
    JsonInit(&item);
    const char* str = "\"\\u0024\"";
    TEST_ASSERT_EQUAL(JSON_PARSE_OK, JsonParse(&item, str));
    TEST_ASSERT_EQUAL(JSON_STRING, item.type);
    TEST_ASSERT_EQUAL_STRING("\x24", item.stringValue);
    JsonFree(&item);

    str = "\"\\u00A2\"";
    JsonInit(&item);
    TEST_ASSERT_EQUAL(JSON_PARSE_OK, JsonParse(&item, str));
    TEST_ASSERT_EQUAL(JSON_STRING, item.type);
    TEST_ASSERT_EQUAL_STRING("\xC2\xA2", item.stringValue);
    JsonFree(&item);

    str = "\"\\u20AC\"";
    JsonInit(&item);
    TEST_ASSERT_EQUAL(JSON_PARSE_OK, JsonParse(&item, str));
    TEST_ASSERT_EQUAL(JSON_STRING, item.type);
    TEST_ASSERT_EQUAL_STRING("\xE2\x82\xAC", item.stringValue);
    JsonFree(&item);

    str = "\"\\uD834\\uDD1E\"";
    JsonInit(&item);
    TEST_ASSERT_EQUAL(JSON_PARSE_OK, JsonParse(&item, str));
    TEST_ASSERT_EQUAL(JSON_STRING, item.type);
    TEST_ASSERT_EQUAL_STRING("\xF0\x9D\x84\x9E", item.stringValue);
    JsonFree(&item);

    str = "\"\\ud834\\udd1e\"";
    JsonInit(&item);
    TEST_ASSERT_EQUAL(JSON_PARSE_OK, JsonParse(&item, str));
    TEST_ASSERT_EQUAL(JSON_STRING, item.type);
    TEST_ASSERT_EQUAL_STRING("\xF0\x9D\x84\x9E", item.stringValue);
    JsonFree(&item);
}
int main(int argc, char *argv[])
{
    UNITY_BEGIN();
    RUN_TEST(TestJsonParseString);
    RUN_TEST(TestJsonParseStringUtf);
    return UNITY_END();
}
