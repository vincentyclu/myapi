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
#include <vector>
#include <string>

zend_class_entry* response_ce;

PHP_METHOD(response, __construct)
{
    zval *data;

    ZEND_PARSE_PARAMETERS_START(1, 1)
        Z_PARAM_ZVAL(data)
    ZEND_PARSE_PARAMETERS_END();

    zend_update_property(response_ce, getThis(), "data", strlen("data"), data);
}

PHP_METHOD(response, output)
{
    zval *data = zend_read_property(response_ce, getThis(), "data", strlen("data"), 0, NULL);

    if (Z_TYPE_P(data) == IS_ARRAY || Z_TYPE_P(data) == IS_OBJECT)
    {
        smart_str buf = {0};
        php_json_encode(&buf, data, 0);
        smart_str_appendc(&buf, '\0');

        RETURN_STR(buf.s);

    }
    else
    {
        RETURN_ZVAL(data, 1, 0);
    }
}

zend_function_entry response_methods[] = {
	PHP_ME(response, __construct,         NULL,     ZEND_ACC_PUBLIC | ZEND_ACC_CTOR)
	PHP_ME(response, output,         NULL,     ZEND_ACC_PUBLIC)

	{NULL, NULL, NULL}
};

MYAPI_STARTUP_FUNCTION(response)
{
    zend_class_entry ce;
    INIT_CLASS_ENTRY(ce, "myapi\\Response", response_methods);
    response_ce = zend_register_internal_class(&ce);

    zend_declare_property_null(response_ce, "data", strlen("data"), ZEND_ACC_PRIVATE);

    return SUCCESS;
}

