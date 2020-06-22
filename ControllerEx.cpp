extern "C" {
    #ifdef HAVE_CONFIG_H
    # include "config.h"
    #endif

    #include "php.h"
    #include "ext/standard/info.h"
    #include "ext/standard/php_var.h"
    #include "ext/json/php_json.h"
    #include "zend_smart_str.h"
}
#include "php_myapi.h"
#include "MyApiTool.h"
#include "ControllerEx.h"
#include "ValidatorEx.h"
#include "ResponseEx.h"
#include "RouterEx.h"

extern zend_class_entry* controller_ce;

Controller Controller::getInstance()
{
    static Controller controller;

    return controller;
}

bool Controller::doValidator(zval& obj)
{
    std::shared_ptr<zval> lawsPtr = MyApiTool::callMethodWithObject(obj, "__validator");
    if (!lawsPtr || Z_TYPE_P(lawsPtr.get()) != IS_ARRAY)
    {
        return true;
    }

    std::string action = Router::getInstance().getAction();
    zval* laws = MyApiTool::getZvalByHashTable(Z_ARRVAL_P(lawsPtr.get()), action.c_str());

    if (laws)
    {
        if (Z_TYPE_P(laws) != IS_ARRAY)
        {
            Response::getInstance().setErrorResult(MYAPI_ERR(200));
            return false;
        }

        zval* get_data_ptr = MyApiTool::getZvalByHashTable(&EG(symbol_table), "_GET");
        zval* post_data_ptr = MyApiTool::getZvalByHashTable(&EG(symbol_table), "_POST");
        zval* file_data_ptr = MyApiTool::getZvalByHashTable(&EG(symbol_table), "_FILES");
        zval* argv_data_ptr = MyApiTool::getZvalByHashTable(&EG(symbol_table), "argv");

        bool isSuccess = Controller::getInstance().doCheck(&obj, laws, get_data_ptr, post_data_ptr, file_data_ptr, argv_data_ptr);

        return isSuccess;
    }

    return true;
}

bool Controller::doCheck(zval *obj, zval *laws, zval *get_data, zval *post_data, zval *file_data, zval *cli_data)
{
    zval result;
    array_init(&result);
    bool isSuccess = Validator::doValidator(laws, get_data, post_data, file_data, cli_data, &result);

    if (isSuccess)
    {
        zend_string *key;
        zend_long num_key;
        zval *val;

        ZEND_HASH_FOREACH_KEY_VAL(Z_ARRVAL(result), num_key, key, val)
            zend_update_property(controller_ce, obj, ZSTR_VAL(key), ZSTR_LEN(key), val);
        ZEND_HASH_FOREACH_END();
    }

    zval_ptr_dtor(&result);

    return isSuccess;
}
