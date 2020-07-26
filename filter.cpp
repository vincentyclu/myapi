extern "C" {
    #ifdef HAVE_CONFIG_H
    # include "config.h"
    #endif

    #include "php.h"
    #include "ext/standard/info.h"
    #include "ext/standard/php_var.h"
}
#include "php_myapi.h"
#include "filter.h"
#include "MyApiTool.h"
#include "RouterEx.h"
#include "ConfigEx.h"
#include "FilterEx.h"
#include "ResponseEx.h"
#include <string>

zend_class_entry* filter_ce;

ZEND_BEGIN_ARG_INFO(arginfo_myapi_filter_begin, 0)
    //ZEND_ARG_TYPE_INFO(0, test_str, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_myapi_filter_after, 0)
    ZEND_ARG_INFO(1, result)
ZEND_END_ARG_INFO()

PHP_METHOD(filter, __construct)
{

}

PHP_METHOD(filter, error)
{
    char *err_msg;
    size_t err_msg_len;
    zend_long code;

    ZEND_PARSE_PARAMETERS_START(2, 2)
        Z_PARAM_STRING(err_msg, err_msg_len)
        Z_PARAM_LONG(code)
    ZEND_PARSE_PARAMETERS_END();

    Response::getInstance().setErrorResult(err_msg, (int) code);
}

zend_function_entry filter_methods[] = {
	PHP_ABSTRACT_ME(filter, begin,         NULL)
	PHP_ABSTRACT_ME(filter, after,         arginfo_myapi_filter_after)
	PHP_ME(filter, error, NULL, ZEND_ACC_PUBLIC)
	PHP_ME(filter, __construct, NULL, ZEND_ACC_CTOR | ZEND_ACC_PUBLIC)

	{NULL, NULL, NULL}
};

MYAPI_STARTUP_FUNCTION(filter)
{
    zend_class_entry ce;
    INIT_CLASS_ENTRY(ce, "myapi\\Filter", filter_methods);

    filter_ce = zend_register_internal_class(&ce);
    zend_declare_property_null(filter_ce, "include_rule", strlen("include_rule"), ZEND_ACC_PUBLIC);
    zend_declare_property_null(filter_ce, "exclude_rule", strlen("exclude_rule"), ZEND_ACC_PUBLIC);

    return SUCCESS;
}
