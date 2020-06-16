extern "C" {
    #ifdef HAVE_CONFIG_H
    # include "config.h"
    #endif

    #include "php.h"
    #include "ext/standard/info.h"
    #include "ext/standard/php_var.h"
    #include "ext/json/php_json.h"
    #include "zend_smart_str.h"
}
#include "php_myapi.h"
#include "MyApiTool.h"
#include "ResponseEx.h"

std::string Response::getErrorResult(std::string errorMsg, int errorCode)
{
    std::string result;
    zval arr;
    array_init(&arr);
    add_assoc_string(&arr, "error_msg", errorMsg.c_str());
    add_assoc_long(&arr, "error_code", errorCode);

    smart_str buf = {0};
    php_json_encode(&buf, &arr, 0);
    smart_str_appendc(&buf, '\0');

    result = ZSTR_VAL(buf.s);
    smart_str_free(&buf);
    zval_ptr_dtor(&arr);

    return result;
}

std::string Response::getJsonString(zval *val)
{
    std::string result = "";

    if (Z_TYPE_P(val) == IS_ARRAY || Z_TYPE_P(val) == IS_OBJECT)
    {
        smart_str buf = {0};
        php_json_encode(&buf, val, 0);
        smart_str_appendc(&buf, '\0');

        result = ZSTR_VAL(buf.s);
        smart_str_free(&buf);
    }

    return result;
}

Response Response::getInstance()
{
    static Response response;

    return response;
}
