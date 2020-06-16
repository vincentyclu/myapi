
extern "C" {
    #ifdef HAVE_CONFIG_H
    # include "config.h"
    #endif

    #include "php.h"
    #include "ext/standard/info.h"
    #include "ext/standard/php_var.h"
}
#include "php_myapi.h"
#include "version.h"
#include "MyApiTool.h"
#include <string>

zend_class_entry* version_ce;

PHP_METHOD(version, __construct)
{

}


PHP_METHOD(version, set)
{
    zval *ver;
    zend_fcall_info fi;
    zend_fcall_info_cache fci;

    ZEND_PARSE_PARAMETERS_START(2, 2)
        Z_PARAM_ARRAY(ver)
        Z_PARAM_FUNC(fi, fci)
    ZEND_PARSE_PARAMETERS_END();

    zval cv_params[] = {*ver};
    std::shared_ptr<zval> cv_ret = MyApiTool::callMethodWithObject(*getThis(), "checkVersion", 1, cv_params);

    if (!cv_ret || Z_TYPE_P(cv_ret.get()) != IS_TRUE)
    {
        RETURN_FALSE;
    }

    zval ret;
    zval *result = zend_read_static_property(version_ce, "result", strlen("result"), 0);
    zval callParams[] = {*result};
    fi.param_count = 1;
    fi.params = callParams;

    zend_fcall_info_call(&fi, &fci, &ret, NULL);

    zend_update_static_property(version_ce, "result", strlen("result"), &ret);
    zval_ptr_dtor(&ret);

    RETURN_TRUE;
}

PHP_METHOD(version, getResult)
{
    zval *ret = zend_read_static_property(version_ce, "result", strlen("result"), 0);

    RETURN_ZVAL(ret, 1, 0);
}


zend_function_entry version_methods[] = {
	PHP_ME(version, __construct,         NULL,     ZEND_ACC_PUBLIC | ZEND_ACC_CTOR)
	PHP_ABSTRACT_ME(version, checkVersion, NULL)
	PHP_ME(version, set, NULL, ZEND_ACC_PROTECTED)
	PHP_ME(version, getResult, NULL, ZEND_ACC_PUBLIC)

	{NULL, NULL, NULL}
};

MYAPI_STARTUP_FUNCTION(version)
{
    zend_class_entry ce;
    INIT_CLASS_ENTRY(ce, "myapi\\Version", version_methods);

    version_ce = zend_register_internal_class(&ce);
    zend_declare_property_null(version_ce, "result", strlen("result"), ZEND_ACC_PUBLIC | ZEND_ACC_STATIC);

    return SUCCESS;
}
