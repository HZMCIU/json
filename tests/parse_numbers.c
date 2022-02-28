#include "json.h"

#include <limits.h>

#define UNITY_INCLUDE_DOUBLE
#include "unity_internals.h"
#include "unity.h"


void setUp()
{

}

void tearDown()
{

}


void TestJsonParseNumberInteger()
{
    Json item;
    TEST_ASSERT_EQUAL_MESSAGE(JSON_PARSE_OK, JsonParse(&item, "314"), "valid integer number 314");
    TEST_ASSERT_EQUAL(JSON_NUMBER_INT, item.type);
    TEST_ASSERT_EQUAL_INT(314, item.intValue);

    /*integer max*/
    TEST_ASSERT_EQUAL_MESSAGE(JSON_PARSE_OK, JsonParse(&item, "9223372036854775807"), "max integer number 9223372036854775807");
    TEST_ASSERT_EQUAL(JSON_NUMBER_INT, item.type);
    TEST_ASSERT_EQUAL_INT64(9223372036854775807LL, item.intValue);

    /*integer min*/
    TEST_ASSERT_EQUAL_MESSAGE(JSON_PARSE_OK, JsonParse(&item, "-9223372036854775808"), "max integer number -9223372036854775808");
    TEST_ASSERT_EQUAL(JSON_NUMBER_INT, item.type);
    long long llmin = -9223372036854775807LL;
    TEST_ASSERT_EQUAL_INT64(llmin - 1, item.intValue);
}

void TestJsonParseNumberValidFloat()
{
    Json item;
    TEST_ASSERT_EQUAL_MESSAGE(JSON_PARSE_OK, JsonParse(&item, "3.14"), "valid float number 3.14");
    TEST_ASSERT_EQUAL(JSON_NUMBER_FLOAT, item.type);
    TEST_ASSERT_EQUAL_DOUBLE(3.14, item.doubleValue);

    TEST_ASSERT_EQUAL_MESSAGE(JSON_PARSE_OK, JsonParse(&item, "-3.14"), "valid float number -3.14");
    TEST_ASSERT_EQUAL(JSON_NUMBER_FLOAT, item.type);
    TEST_ASSERT_EQUAL_DOUBLE(-3.14, item.doubleValue);

    TEST_ASSERT_EQUAL_MESSAGE(JSON_PARSE_OK, JsonParse(&item, "0.14"), "valid float number 0.14");
    TEST_ASSERT_EQUAL(JSON_NUMBER_FLOAT, item.type);
    TEST_ASSERT_EQUAL_DOUBLE(0.14, item.doubleValue);


    TEST_ASSERT_EQUAL_MESSAGE(JSON_PARSE_OK, JsonParse(&item, "3.14e100"), "valid float number 3.14e100");
    TEST_ASSERT_EQUAL(JSON_NUMBER_FLOAT, item.type);
    TEST_ASSERT_EQUAL_DOUBLE(3.14e100, item.doubleValue);

    TEST_ASSERT_EQUAL_MESSAGE(JSON_PARSE_OK, JsonParse(&item, "3.14E+100"), "valid float number 3.14E+100");
    TEST_ASSERT_EQUAL(JSON_NUMBER_FLOAT, item.type);
    TEST_ASSERT_EQUAL_DOUBLE(3.14E+100, item.doubleValue);

}

void TestJsonParseNumberExtremeFloat()
{
    Json item;
    /*the smallest number > 1.*/
    TEST_ASSERT_EQUAL_MESSAGE(JSON_PARSE_OK, JsonParse(&item, "1.0000000000000002"), "the smallest number > 1. 1.0000000000000002");
    TEST_ASSERT_EQUAL(JSON_NUMBER_FLOAT, item.type);
    TEST_ASSERT_EQUAL_DOUBLE(1.0000000000000002, item.doubleValue);

    /*Min. subnormal positive double*/
    TEST_ASSERT_EQUAL_MESSAGE(JSON_PARSE_OK, JsonParse(&item, "4.9406564584124654e-324"), "Min. subnormal positive double. 4.9406564584124654e-324");
    TEST_ASSERT_EQUAL(JSON_NUMBER_FLOAT, item.type);
    TEST_ASSERT_EQUAL_DOUBLE(4.9406564584124654e-324, item.doubleValue);

    /*Max. subnormal double*/
    TEST_ASSERT_EQUAL_MESSAGE(JSON_PARSE_OK, JsonParse(&item, "2.2250738585072009e-308"), "Max. subnormal double. 2.2250738585072009e-308 ");
    TEST_ASSERT_EQUAL(JSON_NUMBER_FLOAT, item.type);
    TEST_ASSERT_EQUAL_DOUBLE(2.2250738585072009e-308, item.doubleValue);

    /*Min. normal positive double*/
    TEST_ASSERT_EQUAL_MESSAGE(JSON_PARSE_OK, JsonParse(&item, "2.2250738585072014e-308"), "Min. normal positive double. 2.2250738585072014e-308");
    TEST_ASSERT_EQUAL(JSON_NUMBER_FLOAT, item.type);
    TEST_ASSERT_EQUAL_DOUBLE(2.2250738585072014e-308, item.doubleValue);

    /*Max. Double*/
    TEST_ASSERT_EQUAL_MESSAGE(JSON_PARSE_OK, JsonParse(&item, "1.7976931348623157e308"), "Max. Double 1.7976931348623157e308");
    TEST_ASSERT_EQUAL(JSON_NUMBER_FLOAT, item.type);
    TEST_ASSERT_EQUAL_DOUBLE(1.7976931348623157e308, item.doubleValue);
}

void TestJsonParseNumberFormatError()
{
    Json item;
    TEST_ASSERT_EQUAL_DOUBLE_MESSAGE(JSON_NUMBER_FORMAT_ERROR, JsonParse(&item, "013"), "invalid number 013");

    TEST_ASSERT_EQUAL_DOUBLE_MESSAGE(JSON_INVALID_VALUE, JsonParse(&item, "+13"), "invalid number +13");

    TEST_ASSERT_EQUAL_DOUBLE_MESSAGE(JSON_NUMBER_FORMAT_ERROR, JsonParse(&item, "3e"), "invalid number 3e");

    TEST_ASSERT_EQUAL_DOUBLE_MESSAGE(JSON_NUMBER_FORMAT_ERROR, JsonParse(&item, "314."), "invalid number 314.");

    TEST_ASSERT_EQUAL_DOUBLE_MESSAGE(JSON_NUMBER_FORMAT_ERROR, JsonParse(&item, "314.e100"), "invalid number 314.e100");

    TEST_ASSERT_EQUAL_DOUBLE_MESSAGE(JSON_NUMBER_FORMAT_ERROR, JsonParse(&item, "3.14e+"), "invalid number 3.14e+");
}
int main(int argc, char *argv[])
{
    UNITY_BEGIN();
    RUN_TEST(TestJsonParseNumberValidFloat);
    RUN_TEST(TestJsonParseNumberExtremeFloat);
    RUN_TEST(TestJsonParseNumberFormatError);
    RUN_TEST(TestJsonParseNumberInteger);
    return UNITY_END();
}
