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

extern zend_class_entry* controller_ce;

Controller Controller::getInstance()
{
    static Controller controller;

    return controller;
}

zval Controller::doCheck(zval *obj, zval *laws, zval *get_data, zval *post_data, zval *file_data, zval *cli_data, bool& isSuccess)
{
    zval result = Validator::doValidator(laws, get_data, post_data, file_data, cli_data, isSuccess);

    if (isSuccess && Z_TYPE(result) == IS_ARRAY)
    {
        zend_string *key;
        zend_long num_key;
        zval *val;

        ZEND_HASH_FOREACH_KEY_VAL(Z_ARRVAL(result), num_key, key, val)
            zend_update_property(controller_ce, obj, ZSTR_VAL(key), ZSTR_LEN(key), val);
        ZEND_HASH_FOREACH_END();
    }

    return result;
}
