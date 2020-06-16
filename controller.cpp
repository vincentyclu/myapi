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

PHP_METHOD(controller, doValidator)
{
    std::shared_ptr<zval> lawsPtr = MyApiTool::callMethodWithObject(*getThis(), "__validator");
    if (!lawsPtr || Z_TYPE_P(lawsPtr.get()) != IS_ARRAY)
    {
        return;
    }

    std::string action = Router::getInstance().getAction();
    std::shared_ptr<zval> actionLawsPtr = MyApiTool::getZvalByHashTable(Z_ARRVAL_P(lawsPtr.get()), action.c_str(), false);

    if (actionLawsPtr)
    {
        zval *laws = actionLawsPtr.get();
        std::shared_ptr<zval> get_data_ptr = MyApiTool::getZvalByHashTable(&EG(symbol_table), "_GET", false);
        std::shared_ptr<zval> post_data_ptr = MyApiTool::getZvalByHashTable(&EG(symbol_table), "_POST", false);
        std::shared_ptr<zval> file_data_ptr = MyApiTool::getZvalByHashTable(&EG(symbol_table), "_FILES", false);
        std::shared_ptr<zval> argv_data_ptr = MyApiTool::getZvalByHashTable(&EG(symbol_table), "argv", false);
        bool is_success;
        zval ret = Controller::getInstance().doCheck(getThis(), laws, get_data_ptr.get(), post_data_ptr.get(), file_data_ptr.get(), argv_data_ptr.get(), is_success);

        if (is_success)
        {
            zval_ptr_dtor(&ret);
            RETURN_TRUE;
        }
        else
        {
            RETURN_ZVAL(&ret, 1, 1);
        }
    }

    RETURN_TRUE;
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

zend_function_entry controller_methods[] = {
	PHP_ME(controller, __construct,         NULL,     ZEND_ACC_PUBLIC | ZEND_ACC_CTOR)
	PHP_ME(controller, getApi,         NULL,     ZEND_ACC_PUBLIC)
	PHP_ME(controller, getModule,         NULL,     ZEND_ACC_PUBLIC)
	PHP_ME(controller, getAction,         NULL,     ZEND_ACC_PUBLIC)
	PHP_ME(controller, doValidator,         NULL,     ZEND_ACC_PUBLIC)

	{NULL, NULL, NULL}
};

MYAPI_STARTUP_FUNCTION(controller)
{
    zend_class_entry ce;
    INIT_CLASS_ENTRY(ce, "myapi\\Controller", controller_methods);
    controller_ce = zend_register_internal_class(&ce);

    return SUCCESS;
}
