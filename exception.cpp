extern "C" {
    #ifdef HAVE_CONFIG_H
    # include "config.h"
    #endif

    #include "php.h"
    #include "ext/standard/info.h"
    #include "ext/standard/php_var.h"
    #include "zend_exceptions.h"
    #include "zend_inheritance.h"
}
#include "php_myapi.h"
#include "exception.h"
#include "MyApiTool.h"
#include <vector>
#include <string>

zend_class_entry* exception_ce;

/*
PHP_METHOD(exception, __construct)
{
    php_printf("fun construct");
    std::vector<zval> v;
    //zval* obj = getThis();
    //MyApiTool::callMethodWithObject(*obj, "\\Test::__construct", v);
}
*/



zend_function_entry exception_methods[] = {

	{NULL, NULL, NULL}
};

MYAPI_STARTUP_FUNCTION(exception)
{
    zend_class_entry ce;
    INIT_CLASS_ENTRY(ce, "myapi\\Exception", exception_methods);
    exception_ce = zend_register_internal_class_ex(&ce, zend_exception_get_default());

    return SUCCESS;
}


