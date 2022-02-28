#include "json.h"

#include <errno.h>
#include <math.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ISDIGIT(ch) ('0' <= (ch) && (ch) <= '9')
#define ISDIGIT1TO9(ch) ('1' <= (ch) && (ch) <= '9')

/**
 * @brief json parsing context.
 */
typedef struct {
    const char* json;
    char* stk;
    size_t top, size;
} JsonContext;

enum {
    JSON_CONTEXT_MEMORY_ERROR,
    JSON_CONTEXT_OK,
    JSON_CONTEXT_FAIL,

    JSON_INVALID_HEX,
    JSON_VALID_HEX
};
#define INIT_STACK_SIZE 64
#define PUTC(ctx, ch)                                    \
    do {                                                 \
        *(char*)JsonContextPush(ctx, sizeof(char)) = ch; \
    } while (0)
int JsonContextInit(JsonContext* ctx, const char* json)
{
    ctx->stk = (char*)malloc(sizeof(char) * INIT_STACK_SIZE);

    if (ctx->stk == NULL) {
        return JSON_CONTEXT_MEMORY_ERROR;
    }

    ctx->size = INIT_STACK_SIZE;
    ctx->top = 0;

    ctx->json = json;
    return JSON_CONTEXT_OK;
}

void* JsonContextPush(JsonContext* ctx, size_t valLen)
{
    if (ctx->size - ctx->top < valLen) {
        size_t expectSize = ctx->size;

        while (expectSize - ctx->top < valLen) {
            expectSize += expectSize / 2;
        }

        char* temp = (char*)realloc(ctx->stk, expectSize);
        if (temp == NULL) {
            return NULL;
        }

        ctx->stk = temp;
    }
    char* tmp = ctx->stk + ctx->top;

    ctx->top += valLen;

    return tmp;
}

void* JsonContextPop(JsonContext* ctx, size_t valLen)
{
    if (valLen > ctx->top) {
        return NULL;
    }

    ctx->top -= valLen;

    return (void*)ctx->stk + ctx->top;
}

void JsonInit(Json* item)
{
    item->type = JSON_NULL;
    item->stringValue = NULL;
    item->child = NULL;
    item->next = NULL;
    item->prev = NULL;
    item->key = NULL;
}

void JsonFree(Json* item)
{
    if (item->type == JSON_STRING) {
        free(item->stringValue);
        item->stringValue = NULL;
    } else if (item->type == JSON_ARRAY) {
        if (item->child != NULL) {
            Json* ptr = item->child->next;
            Json* next = ptr == NULL ? NULL : ptr->next;
            while (ptr != NULL) {
                if (ptr->type == JSON_ARRAY || ptr->type == JSON_OBJECT || ptr->type == JSON_STRING) {
                    JsonFree(ptr);
                    free(ptr);
                } else {
                    free(ptr);
                }
                ptr = next;
                next = next == NULL ? NULL : next->next;
            }
            free(item->child);
            item->child = NULL;
        }
    } else if (item->type == JSON_OBJECT) {
        if (item->child != NULL) {
            Json* ptr = item->child->next;
            Json* next = ptr == NULL ? NULL : ptr->next;
            while (ptr != NULL) {
                free(ptr->key);
                if (ptr->type == JSON_ARRAY || ptr->type == JSON_OBJECT || ptr->type == JSON_STRING) {
                    JsonFree(ptr);
                    free(ptr);
                } else {
                    free(ptr);
                }
                ptr = next;
                next = next == NULL ? NULL : next->next;
            }
            free(item->child);
            item->child = NULL;
        }
    }
}

void JsonParseWhiteSpace(JsonContext* c)
{
    while (*c->json == ' ' || *c->json == '\n' || *c->json == '\r' || *c->json == '\t' || *c->json == '\f')
        c->json++;
}

int JsonParseLiteral(JsonContext* ctx, Json* item, const char* expect, size_t len, JsonType expectType)
{
    size_t expectLen = 0;
    while (ctx->json[expectLen] != '\0' && expectLen < len) {
        expectLen++;
    }

    if (expectLen != len) {
        return JSON_INVALID_VALUE;
    }

    if (strncmp(ctx->json, expect, len) != 0) {
        return JSON_INVALID_VALUE;
    }

    item->type = expectType;
    ctx->json += len;
    return JSON_PARSE_OK;
}

int JsonParseNumber(JsonContext* ctx, Json* item)
{
    int isFloat = 0;
    const char* ch = ctx->json;

    /*check whether is valid json format*/
    if (*ch == '-') {
        ch++;
    }

    if (*ch == '0') {
        ch++;
        if (ISDIGIT1TO9(*ch)) {
            return JSON_NUMBER_FORMAT_ERROR;
        }
    } else if (ISDIGIT1TO9(*ch)) {
        while (ISDIGIT(*ch)) {
            ch++;
        }
    } else {
        return JSON_NUMBER_FORMAT_ERROR;
    }

    if (*ch == '.') {
        isFloat = 1;
        ch++;

        if (ISDIGIT(*ch)) {
            while (ISDIGIT(*ch)) {
                ch++;
            }
        } else {
            return JSON_NUMBER_FORMAT_ERROR;
        }
    }

    if (*ch == 'E' || *ch == 'e') {
        ch++;
        if (*ch == '-' || *ch == '+') {
            ch++;
        }

        if (ISDIGIT(*ch)) {
            while (ISDIGIT(*ch)) {
                ch++;
            }
        } else {
            return JSON_NUMBER_FORMAT_ERROR;
        }
    }
    //else if (*ch != '\0') {
    //    return JSON_NUMBER_FORMAT_ERROR;
    //}

    if (isFloat == 0) {
        errno = 0;
        item->intValue = strtoll(ctx->json, (char**)&ch, 10);
        if (errno == ERANGE) {
            isFloat = 1;
        } else {
            item->type = JSON_NUMBER_INT;
            ctx->json = ch;
            return JSON_PARSE_OK;
        }
    }

    errno = 0;
    if ((item->doubleValue = strtod(ctx->json, (char**)&ch)) == 0.0 || (errno == ERANGE && item->doubleValue == HUGE_VAL)) {
        return JSON_INVALID_VALUE;
    }
    item->type = JSON_NUMBER_FLOAT;
    ctx->json = ch;
    return JSON_PARSE_OK;
}

int JsonParseDecodeHex4(const char* ch, unsigned int* u)
{
    if (ch[0] == '\0' || ch[1] == '\0' || ch[2] == '\0' || ch[3] == '\0')
        return JSON_INVALID_HEX;
    size_t i = 0, base = 16 * 16 * 16;
    *u = 0;
    for (i = 0; i < 4; i++) {
        if ('0' <= ch[i] && ch[i] <= '9') {
            *u += base * (ch[i] - '0');
        } else if ('a' <= ch[i] && ch[i] <= 'f') {
            *u += base * (ch[i] - 'a' + 10);
        } else if ('A' <= ch[i] && ch[i] <= 'F') {
            *u += base * (ch[i] - 'A' + 10);
        }
        base /= 16;
    }
    return JSON_VALID_HEX;
}

void JsonParseEncodeUtf8(JsonContext* ctx, unsigned int u)
{
    if (u <= 0x7F) {
        PUTC(ctx, u);
    } else if (u <= 0x7FF) {
        PUTC(ctx, 0xC0 | (0x1F & (u >> 6)));
        PUTC(ctx, 0x80 | (0x3F & u));
    } else if (u <= 0xFFFF) {
        PUTC(ctx, 0xE0 | (0xF & (u >> 12)));
        PUTC(ctx, 0x80 | (0x3F & (u >> 6)));
        PUTC(ctx, 0x80 | (0x3F & u));
    } else if (u <= 0x10FFFF) {
        PUTC(ctx, 0xF0 | (0x07 & (u >> 18)));
        PUTC(ctx, 0x80 | (0x3F & (u >> 12)));
        PUTC(ctx, 0x80 | (0x3F & (u >> 6)));
        PUTC(ctx, 0x80 | (0x3F & u));
    }
}

int JsonParseRawString(JsonContext* ctx, char** str)
{
    const char* ch = ctx->json;
    size_t preTop = ctx->top, strLen = 0;
    unsigned int high = 0, low = 0, cp = 0;
    int isValid = 0;
    if (*ch != '"') {
        return JSON_INVALID_STRING;
    }
    ch++;
    for (; *ch && !isValid; ch++) {
        switch (*ch) {
        case '\\': {
            ch++;
            switch (*ch) {
            case '\"':
                PUTC(ctx, '\"');
                break;
            case '\\':
                PUTC(ctx, '\\');
                break;
            case '/':
                PUTC(ctx, '/');
                break;
            case 'b':
                PUTC(ctx, '\b');
                break;
            case 'f':
                PUTC(ctx, '\f');
                break;
            case 'n':
                PUTC(ctx, '\n');
                break;
            case 'r':
                PUTC(ctx, '\r');
                break;
            case 't':
                PUTC(ctx, '\t');
                break;
            case 'u': {
                ch++;
                if (JsonParseDecodeHex4(ch, &high) == JSON_INVALID_HEX) {
                    return JSON_STRING_INVALID_UNICODE;
                }
                ch += 4;
                cp = high;

                if (0xD800 <= high && high <= 0xDBFF) {
                    if (*ch != '\\') {
                        return JSON_STRING_INVALID_UNICODE;
                    }
                    ch++;
                    if (*ch != 'u') {
                        return JSON_STRING_INVALID_UNICODE;
                    }
                    ch++;
                    if ((JsonParseDecodeHex4(ch, &low)) == JSON_INVALID_VALUE || low < 0xD800 || low > 0xDFFF) {
                        return JSON_STRING_INVALID_UNICODE;
                    }
                    ch += 4;
                    /*codepoint = 0x10000 + (H − 0xD800) × 0x400 + (L − 0xDC00)*/
                    cp = 0x10000 + (high - 0xD800) * 0x400 + (low - 0xDC00);
                }

                JsonParseEncodeUtf8(ctx, cp);
            } break;
            default:
                return JSON_INVALID_STRING;
            }
        } break;
        case '\"':
            PUTC(ctx, '\0');
            isValid = 1;
            break;
        case '\0':
            return JSON_INVALID_STRING;
        default:
            PUTC(ctx, *ch);
        }
    }

    strLen = ctx->top - preTop;
    if ((*str = (char*)malloc(strLen + 1)) == NULL) {
        return JSON_MEMORY_ERROR;
    }
    memcpy(*str, JsonContextPop(ctx, strLen), strLen);
    *(*str + strLen) = '\0';
    ctx->json = ch;
    return JSON_PARSE_OK;
}
int JsonParseString(JsonContext* ctx, Json* item)
{
    int ret = 0;
    if ((ret != JsonParseRawString(ctx, &item->stringValue)) != JSON_PARSE_OK) {
        return ret;
    }
    item->type = JSON_STRING;
    return JSON_PARSE_OK;
}

int JsonParseValue(JsonContext* ctx, Json* item);
int JsonParseArray(JsonContext* ctx, Json* item)
{
    if (ctx->json[0] != '[') {
        return JSON_INVALID_ARRAY;
    }
    ctx->json++;

    if ((item->child = (Json*)malloc(sizeof(Json))) == NULL) {
        return JSON_MEMORY_ERROR;
    }

    JsonInit(item->child);
    item->size_ = 0;
    item->type = JSON_ARRAY;

    int isExpectComma = 0;
    size_t len = 0;
    Json* value = NULL;
    Json* tail = item->child;
    int ret = 0;
    JsonParseWhiteSpace(ctx);
    for (; ctx->json[0] && ctx->json[0] != ']';) {
        if (isExpectComma) {
            JsonParseWhiteSpace(ctx);
            if (ctx->json[0] != ',') {
                JsonFree(item);
                free(item->child);
                return JSON_INVALID_ARRAY;
            }
            ctx->json++;
        }

        if ((value = (Json*)malloc(sizeof(Json))) == NULL) {
            return JSON_MEMORY_ERROR;
        }

        JsonParseWhiteSpace(ctx);
        if ((ret = JsonParseValue(ctx, value)) != JSON_PARSE_OK) {
            JsonFree(item);
            free(item->child);
            free(value);
            return ret;
        }
        JsonParseWhiteSpace(ctx);

        tail->next = value;
        value->prev = tail;
        value->next = NULL;
        tail = value;
        item->size_ += 1;

        isExpectComma = 1;
    }

    if (ctx->json[0] && ctx->json[0] == ']') {
        item->type = JSON_ARRAY;
        ctx->json++;
        return JSON_PARSE_OK;
    }
    JsonFree(item);
    free(item->child);
    return JSON_INVALID_ARRAY;
}

int JsonGetArrayItem(Json* array, size_t index, Json** item)
{
    if (array->child == NULL || array->child->next == NULL) {
        return JSON_ARRAY_OUT_OF_BOUNDARY;
    }
    Json* ptr = array->child;
    size_t i = 0;
    for (i = 0; i <= index; i++) {
        ptr = ptr->next;
    }
    *item = ptr;
    return JSON_PARSE_OK;
}

int JsonGetArraySize(Json* array)
{
    return array->size_;
}

int JsonParseObject(JsonContext* ctx, Json* item)
{
    if (ctx->json[0] != '{') {
        return JSON_INVALID_OBJECT;
    }
    ctx->json++;

    if ((item->child = (Json*)malloc(sizeof(Json))) == NULL) {
        return JSON_MEMORY_ERROR;
    }
    JsonInit(item->child);

    int isExpectComma = 0;
    Json *value = NULL, *tail = item->child;
    int ret = 0;

    for (; ctx->json[0] && ctx->json[0] != '}';) {
        if (isExpectComma) {
            JsonParseWhiteSpace(ctx);
            if (ctx->json[0] != ',') {
                return JSON_INVALID_OBJECT;
            }
            ctx->json++;
        }

        if ((value = (Json*)malloc(sizeof(Json))) == NULL) {
            return JSON_MEMORY_ERROR;
        }
        JsonInit(value);

        JsonParseWhiteSpace(ctx);
        if ((ret = JsonParseRawString(ctx, &value->key)) != JSON_PARSE_OK) {
            return ret;
        }

        JsonParseWhiteSpace(ctx);
        if (ctx->json[0] != ':') {
            JsonFree(item);
            free(value->stringValue);
            free(value);
            return JSON_INVALID_OBJECT;
        }
        ctx->json++;

        JsonParseWhiteSpace(ctx);
        if ((ret = JsonParseValue(ctx, value)) != JSON_PARSE_OK) {
            JsonFree(item);
            free(value);
            return ret;
        }
        JsonParseWhiteSpace(ctx);

        tail->next = value;
        value->prev = tail;
        value->next = NULL;
        tail = value;
        isExpectComma = 1;
    }

    if (ctx->json[0] != '}') {
        JsonFree(item);
        return JSON_INVALID_OBJECT;
    }
    item->type = JSON_OBJECT;
    return JSON_PARSE_OK;
}

int JsonGetObjectItem(Json* obj, const char* key, Json** item)
{
    if (obj == NULL || obj->child == NULL) {
        return JSON_OBJECT_NOT_FOUND;
    }

    Json* ptr = obj->child->next;
    Json* next = ptr == NULL ? NULL : ptr->next;

    while (ptr != NULL) {
        if (strcmp(key, ptr->key) == 0) {
            *item = ptr;
            return JSON_PARSE_OK;
        }
        ptr = next;
        next = next == NULL ? NULL : next->next;
    }
    return JSON_OBJECT_NOT_FOUND;
}
int JsonParseValue(JsonContext* ctx, Json* item)
{
    switch (*ctx->json) {
    case 'n':
        return JsonParseLiteral(ctx, item, "null", 4, JSON_NULL);
    case 't':
        return JsonParseLiteral(ctx, item, "true", 4, JSON_TRUE);
    case 'f':
        return JsonParseLiteral(ctx, item, "false", 5, JSON_FALSE);
    case '-':
    case '0':
    case '1':
    case '2':
    case '3':
    case '4':
    case '5':
    case '6':
    case '7':
    case '8':
    case '9':
        return JsonParseNumber(ctx, item);
    case '"':
        return JsonParseString(ctx, item);
    case '[':
        return JsonParseArray(ctx, item);
    case '{':
        return JsonParseObject(ctx, item);
    default:
        return JSON_INVALID_VALUE;
    }
    return JSON_INVALID_VALUE;
}

int JsonParse(Json* item, const char* json)
{
    JsonContext ctx;
    JsonContextInit(&ctx, json);
    JsonParseWhiteSpace(&ctx);
    int ret = JsonParseValue(&ctx, item);
    free(ctx.stk);
    return ret;
}

Json* JsonCreateNull()
{
    Json* null = NULL;
    if ((null = (Json*)malloc(sizeof(Json))) == NULL) {
        return NULL;
    }
    JsonInit(null);
    null->type = JSON_NULL;
    return null;
}

Json* JsonCreateFalse()
{
    Json* false = NULL;
    if ((false = (Json*)malloc(sizeof(Json))) == NULL) {
        return NULL;
    }
    JsonInit(false);
    false->type = JSON_FALSE;
    return false;
}

Json* JsonCreateTrue()
{
    Json* true = NULL;
    if ((true = (Json*)malloc(sizeof(Json))) == NULL) {
        return NULL;
    }
    JsonInit(true);
    true->type = JSON_TRUE;
    return true;
}

Json* JsonCreateBool(int boolean)
{
    Json* bool = NULL;
    bool = boolean == 0 ? JsonCreateFalse() : JsonCreateTrue();
    return bool;
}

Json* JsonCreateNumber(double number)
{
    Json* num = NULL;
    if ((num = (Json*)malloc(sizeof(Json))) == NULL) {
        return NULL;
    }
    JsonInit(num);
    num->type = JSON_NUMBER_FLOAT;
    num->doubleValue = number;
    return num;
}

Json* JsonCreateString(const char* string)
{
    Json* str = NULL;
    int len = strlen(string);
    if ((str = (Json*)malloc(sizeof(Json))) == NULL) {
        return NULL;
    }
    JsonInit(str);
    str->type = JSON_STRING;
    str->stringValue = (char*)malloc(len + 1);
    memcpy(str->stringValue, string, len);
    str->stringValue[len] = '\0';
    return str;
}
Json* JsonCreateArray()
{
    Json* array = NULL;
    if ((array = (Json*)malloc(sizeof(Json))) == NULL) {
        return NULL;
    }
    JsonInit(array);
    array->type = JSON_ARRAY;
    if ((array->child = (Json*)malloc(sizeof(Json))) == NULL) {
        free(array);
        return NULL;
    }
    JsonInit(array->child);
    return array;
}

Json* JsonCreateObject()
{
    Json* object = NULL;
    if ((object = (Json*)malloc(sizeof(Json))) == NULL) {
        return NULL;
    }
    JsonInit(object);
    object->type = JSON_OBJECT;
    if ((object->child = (Json*)malloc(sizeof(Json))) == NULL) {
        free(object);
        return NULL;
    }
    JsonInit(object->child);
    return object;
}

int JsonAddItemToArray(Json* array, Json* item)
{
    if (item == NULL) {
        return JSON_INVALID_ITEM;
    }
    item->next = array->child->next;
    if (array->child->next != NULL) {
        array->child->next->prev = item;
    }
    array->child->next = item;
    item->prev = array->child;
    return JSON_ADD_OK;
}

int JsonAddItemToObject(Json* object, const char* key, Json* item)
{
    if (item == NULL) {
        return JSON_INVALID_ITEM;
    }
    int len = strlen(key);
    item->key = (char*)malloc(len + 1);
    memcpy(item->key, key, len);
    item->key[len] = '\0';
    item->next = object->child->next;
    if (object->child->next != NULL) {
        object->child->next->prev = item;
    }
    object->child->next = item;
    item->prev = object->child;
    return JSON_ADD_OK;
}
