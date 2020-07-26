extern "C" {
    #ifdef HAVE_CONFIG_H
    # include "config.h"
    #endif

    #include "php.h"
    #include "ext/standard/info.h"
    #include "ext/standard/php_var.h"

}
#include "php_myapi.h"
#include "controller.h"
#include "MyApiTool.h"
#include "RouterEx.h"
#include "ValidatorEx.h"
#include "ControllerEx.h"
#include "RouterEx.h"
#include <vector>
#include <string>

zend_class_entry* controller_ce;

PHP_METHOD(controller, __construct)
{

}

PHP_METHOD(controller, getApi)
{
    std::string api = Router::getInstance().getApi();

    RETURN_STRING(api.c_str());
}

PHP_METHOD(controller, getModule)
{
    std::string module = Router::getInstance().getModule();

    RETURN_STRING(module.c_str());
}

PHP_METHOD(controller, getAction)
{
    std::string action = Router::getInstance().getAction();

    RETURN_STRING(action.c_str());
}

PHP_METHOD(controller, __validator)
{

}

zend_function_entry controller_methods[] = {
	PHP_ME(controller, __construct,         NULL,     ZEND_ACC_PUBLIC | ZEND_ACC_CTOR)
	PHP_ME(controller, getApi,         NULL,     ZEND_ACC_PUBLIC)
	PHP_ME(controller, getModule,         NULL,     ZEND_ACC_PUBLIC)
	PHP_ME(controller, getAction,         NULL,     ZEND_ACC_PUBLIC)
	PHP_ME(controller, __validator,         NULL,     ZEND_ACC_PUBLIC)

	{NULL, NULL, NULL}
};

MYAPI_STARTUP_FUNCTION(controller)
{
    zend_class_entry ce;
    INIT_CLASS_ENTRY(ce, "myapi\\Controller", controller_methods);
    controller_ce = zend_register_internal_class(&ce);

    return SUCCESS;
}
