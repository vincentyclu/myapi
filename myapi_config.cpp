extern "C" {
    #ifdef HAVE_CONFIG_H
    # include "config.h"
    #endif

    #include "php.h"
    #include "ext/standard/info.h"
    #include "ext/standard/php_var.h"
    #include "php_myapi.h"
}

#include "myapi_config.h"
#include "MyApiTool.h"
#include <vector>
#include <string>

zend_class_entry* config_ce;

PHP_METHOD(config, __construct)
{
    zval *file_path;

    ZEND_PARSE_PARAMETERS_START(1, 1)
        Z_PARAM_ZVAL(file_path)
    ZEND_PARSE_PARAMETERS_END();

    std::vector<zval> v;
    v.push_back(*file_path);

    zval ret = MyApiTool::callFunction("\\myapi\\Api::import", v);

    ret.u1.v.type = IS_ARRAY;

    zend_update_property(config_ce, getThis(), "data", strlen("data"), &ret);

    zval_ptr_dtor(&ret);
}

PHP_METHOD(config, get)
{
    zval *rv;
    zval *val = zend_read_property(config_ce, getThis(), "data", strlen("data"), 0, rv);

    zend_string *key;
    ZEND_PARSE_PARAMETERS_START(1, 1)
        Z_PARAM_STR(key)
    ZEND_PARSE_PARAMETERS_END();

    zval *ret = zend_hash_find(Z_ARRVAL_P(val), key);

    RETURN_ZVAL(ret, 1, 0);
}

zend_function_entry config_methods[] = {
	PHP_ME(config, __construct,         NULL,     ZEND_ACC_PUBLIC | ZEND_ACC_CTOR)
	PHP_ME(config, get,         NULL,     ZEND_ACC_PUBLIC)

	{NULL, NULL, NULL}
};

MYAPI_STARTUP_FUNCTION(config)
{
    zend_class_entry ce;
    INIT_CLASS_ENTRY(ce, "myapi\\Config", config_methods);
    config_ce = zend_register_internal_class(&ce);

    zend_declare_property_null(config_ce, "data", strlen("data"), ZEND_ACC_PUBLIC);

    return SUCCESS;
}

