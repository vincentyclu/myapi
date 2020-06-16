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
    //Router::getInstance().init();
}

PHP_METHOD(router, getApi)
{
    zval *ret = zend_read_static_property(router_ce, "api", strlen("api"), 0);

    RETURN_ZVAL(ret, 1, 0);
}

PHP_METHOD(router, getModule)
{
    zval *ret = zend_read_static_property(router_ce, "module", strlen("module"), 0);

    RETURN_ZVAL(ret, 1, 0);
}

PHP_METHOD(router, getAction)
{
    zval *ret = zend_read_static_property(router_ce, "action", strlen("action"), 0);

    RETURN_ZVAL(ret, 1, 0);
}

PHP_METHOD(router, getController)
{
    zval *ret = zend_read_static_property(router_ce, "controller", strlen("controller"), 0);

    RETURN_ZVAL(ret, 1, 0);
}

zend_function_entry router_methods[] = {
	PHP_ME(router, __construct,         NULL,     ZEND_ACC_PUBLIC | ZEND_ACC_CTOR)
	PHP_ME(router, getApi,         NULL,     ZEND_ACC_PUBLIC)
	PHP_ME(router, getModule,         NULL,     ZEND_ACC_PUBLIC)
	PHP_ME(router, getAction,         NULL,     ZEND_ACC_PUBLIC)
	PHP_ME(router, getController,         NULL,     ZEND_ACC_PUBLIC)

	{NULL, NULL, NULL}
};

MYAPI_STARTUP_FUNCTION(router)
{
    zend_class_entry ce;
    INIT_CLASS_ENTRY(ce, "myapi\\Router", router_methods);
    router_ce = zend_register_internal_class(&ce);

    zend_declare_property_null(router_ce, "php_sapi", strlen("php_sapi"), ZEND_ACC_PRIVATE | ZEND_ACC_STATIC);
    zend_declare_property_null(router_ce, "api", strlen("api"), ZEND_ACC_PRIVATE | ZEND_ACC_STATIC);
    zend_declare_property_null(router_ce, "module", strlen("module"), ZEND_ACC_PRIVATE | ZEND_ACC_STATIC);
    zend_declare_property_null(router_ce, "action", strlen("action"), ZEND_ACC_PRIVATE | ZEND_ACC_STATIC);
    zend_declare_property_null(router_ce, "controller", strlen("controller"), ZEND_ACC_PRIVATE | ZEND_ACC_STATIC);
    zend_declare_property_null(router_ce, "version_class", strlen("version_class"), ZEND_ACC_PRIVATE | ZEND_ACC_STATIC);

    return SUCCESS;
}

