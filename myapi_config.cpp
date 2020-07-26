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
#include "ConfigEx.h"
#include <string>

zend_class_entry* config_ce;


PHP_METHOD(config, __construct)
{

}

PHP_METHOD(config, get)
{
    char *file;
    size_t file_len;
    char *key = NULL;
    size_t key_len;
    zend_bool is_env = true;
    ZEND_PARSE_PARAMETERS_START(1, 3)
        Z_PARAM_STRING(file, file_len)
        Z_PARAM_OPTIONAL
        Z_PARAM_STRING(key, key_len)
        Z_PARAM_BOOL(is_env)
    ZEND_PARSE_PARAMETERS_END();

    std::string k = (key == NULL) ? "" : key;
    zval *data = Config::getInstance().getConfig(file, k, (bool) is_env);

    if (!data)
    {
        RETURN_NULL();
    }

    RETURN_ZVAL(data, 1, 0);
}

PHP_METHOD(config, getObject)
{
    zval *val = zend_read_static_property(config_ce, "config_obj", strlen("config_obj"), 0);

    RETURN_ZVAL(val, 1, 0);
}

PHP_METHOD(config, getLang)
{
    char *key;
    size_t key_len;
    char *lang = NULL;
    size_t lang_len;

    ZEND_PARSE_PARAMETERS_START(1, 2)
        Z_PARAM_STRING(key, key_len)
        Z_PARAM_OPTIONAL
        Z_PARAM_STRING(lang, lang_len)
    ZEND_PARSE_PARAMETERS_END();

    std::string result = Config::getInstance().getLang(key, lang ? lang : "");

    RETURN_STRING(result.c_str());
}

PHP_METHOD(config, getErrorMsg)
{
    zend_long code;

    ZEND_PARSE_PARAMETERS_START(1, 1)
        Z_PARAM_LONG(code)
    ZEND_PARSE_PARAMETERS_END();

    std::string result = Config::getInstance().getErrorMsg((int) code);

    RETURN_STRING(result.c_str());
}

PHP_METHOD(config, setUserConfig)
{
    char *key;
    size_t key_len;
    zval *val;

    ZEND_PARSE_PARAMETERS_START(2, 2)
        Z_PARAM_STRING(key, key_len)
        Z_PARAM_ZVAL(val)
    ZEND_PARSE_PARAMETERS_END();

    zval *user_data = zend_read_static_property(config_ce, "user_data", strlen("user_data"), 0);

    if (Z_TYPE_P(user_data) == IS_NULL)
    {
        array_init(user_data);
    }

    add_assoc_zval(user_data, key, val);
}

PHP_METHOD(config, getUserConfig)
{
    char *key;
    size_t key_len;

    ZEND_PARSE_PARAMETERS_START(1, 1)
        Z_PARAM_STRING(key, key_len)
    ZEND_PARSE_PARAMETERS_END();

    zval *user_data = zend_read_static_property(config_ce, "user_data", strlen("user_data"), 0);

    if (Z_TYPE_P(user_data) != IS_ARRAY)
    {
        RETURN_NULL();
    }

    zval *ret = MyApiTool::getZvalByHashTable(Z_ARRVAL_P(user_data), key);

    if (!ret)
    {
        RETURN_NULL();
    }

    RETURN_ZVAL(ret, 1, 0);
}

zend_function_entry config_methods[] = {
	PHP_ME(config, __construct,         NULL,     ZEND_ACC_PUBLIC | ZEND_ACC_CTOR)
	PHP_ME(config, get,         NULL,     ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
	PHP_ME(config, getObject,         NULL,     ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
	PHP_ME(config, getLang,         NULL,     ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
	PHP_ME(config, getErrorMsg,         NULL,     ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)

	PHP_ME(config, setUserConfig,         NULL,     ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
	PHP_ME(config, getUserConfig,         NULL,     ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)

	{NULL, NULL, NULL}
};

MYAPI_STARTUP_FUNCTION(config)
{
    zend_class_entry ce;
    INIT_CLASS_ENTRY(ce, "myapi\\Config", config_methods);
    config_ce = zend_register_internal_class(&ce);

    zend_declare_property_null(config_ce, "user_data", strlen("user_data"), ZEND_ACC_PUBLIC | ZEND_ACC_STATIC);
    zend_declare_property_null(config_ce, "config_obj", strlen("config_obj"), ZEND_ACC_PRIVATE | ZEND_ACC_STATIC);

    return SUCCESS;
}

