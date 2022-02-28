#ifndef JSON_H__
#define JSON_H__

#include <stddef.h>
/**
 * @brief JSON type enumeration.
 */
typedef enum {
    JSON_NULL,
    JSON_FALSE,
    JSON_TRUE,
    JSON_NUMBER_INT,
    JSON_NUMBER_FLOAT,
    JSON_STRING,
    JSON_OBJECT,
    JSON_ARRAY
} JsonType;

/**
 * @brief JSON data structure.
 */
typedef struct Json {
    JsonType type;
    union {
        long long intValue;
        double doubleValue;
        char* stringValue;
        size_t size_;
    };

    char* key;
    struct Json* child;
    struct Json* next;
    struct Json* prev;
} Json;

/**
 * @brief JSON parse error value.
 */
enum {
    JSON_PARSE_OK,
    JSON_INVALID_VALUE,
    JSON_NUMBER_FORMAT_ERROR,
    JSON_INVALID_STRING,
    JSON_STRING_INVALID_UNICODE,
    JSON_MEMORY_ERROR,
    JSON_INVALID_ARRAY,
    JSON_ARRAY_OUT_OF_BOUNDARY,
    JSON_INVALID_OBJECT,
    JSON_OBJECT_NOT_FOUND,
    JSON_INVALID_ITEM,
    JSON_ADD_OK
};

int JsonParse(Json* item, const char* json);

void JsonInit(Json* item);
void JsonFree(Json* item);

int JsonGetArrayItem(Json* array, size_t index, Json** item);
int JsonGetArraySize(Json* array);

int JsonGetObjectItem(Json* obj, const char* key, Json** item);

/*TODO*/
/* Json* JsonCreateIntArray(const int *numbers, int count);
Json* JsonCreateFloatArray(const float* numbers, int count);
Json* JsonCreateDoubleArray(const double* numbers, int count);
Json* JsonCreateStringArray(const char *const * strings, int count); */

Json* JsonCreateNull();
Json* JsonCreateFalse();
Json* JsonCreateTrue();
Json* JsonCreateBool(int boolean);
Json* JsonCreateNumber(double number);
Json* JsonCreateString(const char* string);

Json* JsonCreateObject();
Json* JsonCreateArray();

int JsonAddItemToArray(Json* array, Json* item);
int JsonAddItemToObject(Json* object, const char* key, Json* item);

/*TODO*/
int JsonPrint(Json* item);
#endif
