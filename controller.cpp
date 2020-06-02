extern "C" {
    #ifdef HAVE_CONFIG_H
    # include "config.h"
    #endif

    #include "php.h"
    #include "ext/standard/info.h"
    #include "ext/standard/php_var.h"
    #include "php_myapi.h"
}

#include "controller.h"
#include "MyApiTool.h"
#include <vector>
#include <string>

zend_class_entry* controller_ce;

PHP_METHOD(controller, __construct)
{

}

PHP_METHOD(controller, get)
{
/*
    zval val;
    val.u1.v.type = IS_ARRAY;
    //HashTable *table = zend_rebuild_symbol_table(); local var
    Z_ARRVAL(val) = &EG(symbol_table);

    RETURN_ZVAL(&val, 1, 0);*/

    zend_string *str;

    ZEND_PARSE_PARAMETERS_START(1, 1)
        Z_PARAM_STR(str)
    ZEND_PARSE_PARAMETERS_END();

    zend_string *key_get = zend_string_init("_GET", strlen("_GET"), 0);
    zend_string *key_post = zend_string_init("_POST", strlen("_POST"), 0);
    zval *get_data = zend_hash_find(&EG(symbol_table), key_get);
    zval *post_data = zend_hash_find(&EG(symbol_table), key_post);
    //zend_bool val = zend_hash_exists(&EG(symbol_table), key_get);
    //php_var_dump(get_data, 0);
    zend_update_property(controller_ce, getThis(), "_GET", strlen("_GET"), get_data);
    zend_update_property(controller_ce, getThis(), "_POST", strlen("_POST"), post_data);
    zend_string_release(key_get);
    zend_string_release(key_post);

    if (!zend_hash_exists(Z_ARRVAL_P(get_data), str))
    {
        RETURN_NULL();
    }

    zval *data = zend_hash_find(Z_ARRVAL_P(get_data), str);

    RETURN_ZVAL(data, 1, 0);
}

zend_function_entry controller_methods[] = {
	PHP_ME(controller, __construct,         NULL,     ZEND_ACC_PUBLIC | ZEND_ACC_CTOR)
	PHP_ME(controller, get,         NULL,     ZEND_ACC_PUBLIC)

	{NULL, NULL, NULL}
};

MYAPI_STARTUP_FUNCTION(controller)
{
    zend_class_entry ce;
    INIT_CLASS_ENTRY(ce, "myapi\\Controller", controller_methods);
    controller_ce = zend_register_internal_class(&ce);

    zend_declare_property_null(controller_ce, "_GET", strlen("_GET"), ZEND_ACC_PUBLIC);
    zend_declare_property_null(controller_ce, "_POST", strlen("_POST"), ZEND_ACC_PUBLIC);

    return SUCCESS;
}
