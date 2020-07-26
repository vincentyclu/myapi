extern "C" {
    #ifdef HAVE_CONFIG_H
    # include "config.h"
    #endif

    #include "php.h"
    #include "ext/standard/info.h"
    #include "ext/standard/php_var.h"
}
#include "php_myapi.h"
#include "router.h"
#include "MyApiTool.h"
#include "RouterEx.h"
#include <string>

zend_class_entry* router_ce;

PHP_METHOD(router, __construct)
{

}

PHP_METHOD(router, getApi)
{
    ZEND_PARSE_PARAMETERS_NONE();

    zval *ret = zend_read_static_property(router_ce, "api", strlen("api"), 0);

    RETURN_ZVAL(ret, 1, 0);
}

PHP_METHOD(router, getModule)
{
    ZEND_PARSE_PARAMETERS_NONE();

    zval *ret = zend_read_static_property(router_ce, "module", strlen("module"), 0);

    RETURN_ZVAL(ret, 1, 0);
}

PHP_METHOD(router, getAction)
{
    ZEND_PARSE_PARAMETERS_NONE();

    zval *ret = zend_read_static_property(router_ce, "action", strlen("action"), 0);

    RETURN_ZVAL(ret, 1, 0);
}

PHP_METHOD(router, getController)
{
    ZEND_PARSE_PARAMETERS_NONE();

    std::string controller = Router::getInstance().getController();

    RETURN_STRING(controller.c_str());
}

PHP_METHOD(router, getPathInfo)
{
    ZEND_PARSE_PARAMETERS_NONE();

    zval *path_info = zend_read_static_property(router_ce, "path_info", strlen("path_info"), 0);

    RETURN_ZVAL(path_info, 1, 0);
}

PHP_METHOD(router, setApi)
{
    char *api;
    size_t api_len;
    ZEND_PARSE_PARAMETERS_START(1, 1)
        Z_PARAM_STRING(api, api_len)
    ZEND_PARSE_PARAMETERS_END();

    zend_update_static_property_string(router_ce, "api", strlen("api"), api);
}

PHP_METHOD(router, setModule)
{
    char *module;
    size_t module_len;
    ZEND_PARSE_PARAMETERS_START(1, 1)
        Z_PARAM_STRING(module, module_len)
    ZEND_PARSE_PARAMETERS_END();

    zend_update_static_property_string(router_ce, "module", strlen("module"), module);
}

PHP_METHOD(router, setAction)
{
    char *action;
    size_t action_len;
    ZEND_PARSE_PARAMETERS_START(1, 1)
        Z_PARAM_STRING(action, action_len)
    ZEND_PARSE_PARAMETERS_END();

    zend_update_static_property_string(router_ce, "action", strlen("action"), action);
}

PHP_METHOD(router, set)
{
    char *path_info_str;
    size_t path_info_str_len;
    zval *router_info;

    ZEND_PARSE_PARAMETERS_START(2, 2)
        Z_PARAM_STRING(path_info_str, path_info_str_len)
        Z_PARAM_ARRAY(router_info)
    ZEND_PARSE_PARAMETERS_END();

    zval *path_info = zend_read_static_property(router_ce, "path_info", strlen("path_info"), 0);

    if (Z_TYPE_P(path_info) != IS_STRING)
    {
        return;
    }

    if (std::string(Z_STRVAL_P(path_info)) == std::string(path_info_str))
    {
        zval *api = MyApiTool::getZvalByHashTable(Z_ARRVAL_P(router_info), "api");

        if (api && Z_TYPE_P(api) == IS_STRING && Z_STRLEN_P(api) > 0)
        {
            zend_update_static_property(router_ce, "api", strlen("api"), api);
        }

        zval *module = MyApiTool::getZvalByHashTable(Z_ARRVAL_P(router_info), "module");

        if (module && Z_TYPE_P(module) == IS_STRING && Z_STRLEN_P(module) > 0)
        {
            zend_update_static_property(router_ce, "module", strlen("module"), module);
        }

        zval *action = MyApiTool::getZvalByHashTable(Z_ARRVAL_P(router_info), "action");

        if (action && Z_TYPE_P(action) == IS_STRING && Z_STRLEN_P(action) > 0)
        {
            zend_update_static_property(router_ce, "action", strlen("action"), action);
        }
    }
}

zend_function_entry router_methods[] = {
	PHP_ME(router, __construct,   NULL,     ZEND_ACC_PUBLIC | ZEND_ACC_CTOR)
	PHP_ME(router, getApi,        NULL,     ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
	PHP_ME(router, getModule,     NULL,     ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
	PHP_ME(router, getAction,     NULL,     ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
	PHP_ME(router, getController, NULL,     ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
	PHP_ME(router, getPathInfo,   NULL,     ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)

	PHP_ME(router, setApi,        NULL,     ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
	PHP_ME(router, setModule,     NULL,     ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
	PHP_ME(router, setAction,     NULL,     ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)

	PHP_ME(router, set,     NULL,     ZEND_ACC_PUBLIC)

	{NULL, NULL, NULL}
};

MYAPI_STARTUP_FUNCTION(router)
{
    zend_class_entry ce;
    INIT_CLASS_ENTRY(ce, "myapi\\Router", router_methods);
    router_ce = zend_register_internal_class(&ce);

    zend_declare_property_null(router_ce, "php_sapi", strlen("php_sapi"), ZEND_ACC_PRIVATE | ZEND_ACC_STATIC);
    zend_declare_property_null(router_ce, "router_obj", strlen("router_obj"), ZEND_ACC_PRIVATE | ZEND_ACC_STATIC);
    zend_declare_property_null(router_ce, "api", strlen("api"), ZEND_ACC_PRIVATE | ZEND_ACC_STATIC);
    zend_declare_property_null(router_ce, "module", strlen("module"), ZEND_ACC_PRIVATE | ZEND_ACC_STATIC);
    zend_declare_property_null(router_ce, "action", strlen("action"), ZEND_ACC_PRIVATE | ZEND_ACC_STATIC);
    zend_declare_property_null(router_ce, "path_info", strlen("path_info"), ZEND_ACC_PRIVATE | ZEND_ACC_STATIC);

    return SUCCESS;
}

