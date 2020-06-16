extern "C" {
    #ifdef HAVE_CONFIG_H
    # include "config.h"
    #endif

    #include "php.h"
    #include "ext/standard/info.h"
    #include "ext/standard/php_var.h"
}

#include "php_myapi.h"
#include "MyApiTool.h"
#include "LogEx.h"
#include <string>

zend_class_entry* log_ce;

PHP_METHOD(log, __construct)
{

}

PHP_METHOD(log, debug)
{
    char *msg;
    size_t msg_len;

    ZEND_PARSE_PARAMETERS_START(1, 1)
        Z_PARAM_STRING(msg, msg_len);
    ZEND_PARSE_PARAMETERS_END();

    bool ret = Log::getInstance().writeLog(msg, "debug");

    RETURN_BOOL(ret);
}

PHP_METHOD(log, info)
{
    char *msg;
    size_t msg_len;

    ZEND_PARSE_PARAMETERS_START(1, 1)
        Z_PARAM_STRING(msg, msg_len);
    ZEND_PARSE_PARAMETERS_END();

    bool ret = Log::getInstance().writeLog(msg, "info");

    RETURN_BOOL(ret);
}

PHP_METHOD(log, warning)
{
    char *msg;
    size_t msg_len;

    ZEND_PARSE_PARAMETERS_START(1, 1)
        Z_PARAM_STRING(msg, msg_len);
    ZEND_PARSE_PARAMETERS_END();

    bool ret = Log::getInstance().writeLog(msg, "warning");

    RETURN_BOOL(ret);
}

PHP_METHOD(log, fatal)
{
    char *msg;
    size_t msg_len;

    ZEND_PARSE_PARAMETERS_START(1, 1)
        Z_PARAM_STRING(msg, msg_len);
    ZEND_PARSE_PARAMETERS_END();

    bool ret = Log::getInstance().writeLog(msg, "fatal");

    RETURN_BOOL(ret);
}

zend_function_entry log_methods[] = {
	PHP_ME(log, __construct,         NULL,     ZEND_ACC_PUBLIC | ZEND_ACC_CTOR)
	PHP_ME(log, debug,         NULL,     ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
	PHP_ME(log, info,         NULL,     ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
	PHP_ME(log, warning,         NULL,     ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
	PHP_ME(log, fatal,         NULL,     ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)

	{NULL, NULL, NULL}
};

MYAPI_STARTUP_FUNCTION(log)
{
    zend_class_entry ce;
    INIT_CLASS_ENTRY(ce, "myapi\\Log", log_methods);
    log_ce = zend_register_internal_class(&ce);

    zend_declare_property_null(log_ce, "log_type", strlen("log_type"), ZEND_ACC_PRIVATE | ZEND_ACC_STATIC);

    return SUCCESS;
}

