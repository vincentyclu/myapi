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
#include "response.h"
#include "MyApiTool.h"
#include "ResponseEx.h"
#include <string>

zend_class_entry* response_ce;

PHP_METHOD(response, __construct)
{

}

PHP_METHOD(response, output)
{
    std::string output = Response::getInstance().output();

    RETURN_STRING(output.c_str());
}

PHP_METHOD(response, getErrorMsg)
{
    zval *error_msg = zend_read_static_property(response_ce, "error_msg", strlen("error_msg"), 0);

    if (error_msg)
    {
        RETURN_ZVAL(error_msg, 1, 0);
    }
}

PHP_METHOD(response, getErrorCode)
{
    zval *error_code = zend_read_static_property(response_ce, "error_code", strlen("error_code"), 0);

    if (error_code && Z_TYPE_P(error_code) != IS_NULL)
    {
        RETURN_ZVAL(error_code, 1, 0);
    }

    RETURN_LONG(0);
}

PHP_METHOD(response, getResult)
{
    zval *result = zend_read_static_property(response_ce, "result", strlen("result"), 0);

    if (result)
    {
        RETURN_ZVAL(result, 1, 0);
    }
}

zend_function_entry response_methods[] = {
	PHP_ME(response, __construct,         NULL,     ZEND_ACC_PUBLIC | ZEND_ACC_CTOR)
	PHP_ME(response, output,         NULL,     ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)

	PHP_ME(response, getErrorMsg,         NULL,     ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
	PHP_ME(response, getErrorCode,         NULL,     ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
	PHP_ME(response, getResult,         NULL,     ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)

	{NULL, NULL, NULL}
};

MYAPI_STARTUP_FUNCTION(response)
{
    zend_class_entry ce;
    INIT_CLASS_ENTRY(ce, "myapi\\Response", response_methods);
    response_ce = zend_register_internal_class(&ce);

    zend_declare_property_null(response_ce, "error_msg", strlen("error_msg"), ZEND_ACC_PRIVATE | ZEND_ACC_STATIC);
    zend_declare_property_null(response_ce, "error_code", strlen("error_code"), ZEND_ACC_PRIVATE | ZEND_ACC_STATIC);
    zend_declare_property_null(response_ce, "result", strlen("result"), ZEND_ACC_PRIVATE | ZEND_ACC_STATIC);
    zend_declare_property_null(response_ce, "is_set_header", strlen("is_set_header"), ZEND_ACC_PRIVATE | ZEND_ACC_STATIC);


    return SUCCESS;
}

