extern "C" {
    #ifdef HAVE_CONFIG_H
    # include "config.h"
    #endif

    #include "php.h"
    #include "ext/standard/info.h"
    #include "ext/standard/php_var.h"
}
#include "php_myapi.h"
#include "myapi_config.h"
#include "MyApiTool.h"
#include "ApiEx.h"
#include <vector>
#include <string>

zend_class_entry* config_ce;


PHP_METHOD(config, __construct)
{
    zval *file_path;
    zend_bool use_env = false;

    ZEND_PARSE_PARAMETERS_START(1, 2)
        Z_PARAM_ZVAL(file_path)
        Z_PARAM_OPTIONAL
        Z_PARAM_BOOL(use_env)
    ZEND_PARSE_PARAMETERS_END();

    std::string api_path = Api::getInstance().getApiPath();
    std::string env = Api::getInstance().getEnv();

    if (Z_TYPE_P(file_path) != IS_STRING || api_path == "")
    {
        return;
    }

    std::string filePath;

    if (use_env)
    {
        filePath = api_path + "/config/" + env + "/" + std::string(Z_STRVAL_P(file_path)) + std::string(".php");
    }
    else
    {
        filePath = api_path + "/config/" + std::string(Z_STRVAL_P(file_path)) + std::string(".php");
    }

    std::shared_ptr<zval> retPtr = Api::getInstance().import(filePath);

    if (Z_TYPE_P(retPtr.get()) != IS_ARRAY)
    {
        MyApiTool::throwException("config file error", 10);
        return;
    }

    zend_update_property(config_ce, getThis(), "data", strlen("data"), retPtr.get());
}

PHP_METHOD(config, get)
{
    zval *rv;
    zval *val = zend_read_property(config_ce, getThis(), "data", strlen("data"), 0, rv);

    char *key;
    size_t len;
    ZEND_PARSE_PARAMETERS_START(1, 1)
        Z_PARAM_STRING(key, len)
    ZEND_PARSE_PARAMETERS_END();

    std::shared_ptr<zval> retPtr = MyApiTool::getZvalByHashTableEx(Z_ARRVAL_P(val), key, false);

    if (!retPtr)
    {
        RETURN_NULL();
    }

    RETURN_ZVAL(retPtr.get(), 1, 0);
}

PHP_METHOD(config, getAll)
{
    zval *val = zend_read_property(config_ce, getThis(), "data", strlen("data"), 0, NULL);

    if (!val)
    {
        RETURN_NULL();
    }

    RETURN_ZVAL(val, 1, 0);
}

zend_function_entry config_methods[] = {
	PHP_ME(config, __construct,         NULL,     ZEND_ACC_PUBLIC | ZEND_ACC_CTOR)
	PHP_ME(config, get,         NULL,     ZEND_ACC_PUBLIC)
	PHP_ME(config, getAll,         NULL,     ZEND_ACC_PUBLIC)

	{NULL, NULL, NULL}
};

MYAPI_STARTUP_FUNCTION(config)
{
    zend_class_entry ce;
    INIT_CLASS_ENTRY(ce, "myapi\\Config", config_methods);
    config_ce = zend_register_internal_class(&ce);

    zend_declare_property_null(config_ce, "data", strlen("data"), ZEND_ACC_PRIVATE);
    zend_declare_property_null(config_ce, "myapi_data", strlen("myapi_data"), ZEND_ACC_PUBLIC | ZEND_ACC_STATIC);

    return SUCCESS;
}

