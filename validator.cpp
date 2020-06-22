extern "C" {
    #ifdef HAVE_CONFIG_H
    # include "config.h"
    #endif

    #include "php.h"
    #include "ext/standard/info.h"
    #include "ext/standard/php_var.h"

}

#include "php_myapi.h"
#include "validator.h"
#include "MyApiTool.h"
#include "ValidatorEx.h"
#include <vector>
#include <string>

zend_class_entry* validator_ce;

PHP_METHOD(validator, __construct)
{
    zval *laws;

    ZEND_PARSE_PARAMETERS_START(1, 1)
        Z_PARAM_ARRAY(laws)
    ZEND_PARSE_PARAMETERS_END();

    zend_update_property(validator_ce, getThis(), "laws", strlen("laws"), laws);
}

PHP_METHOD(validator, doCheck)
{
    zval *laws = zend_read_property(validator_ce, getThis(), "laws", strlen("laws"), 0, NULL);
    zval *get_data = zend_read_property(validator_ce, getThis(), "get_data", strlen("get_data"), 0, NULL);
    zval *post_data = zend_read_property(validator_ce, getThis(), "post_data", strlen("post_data"), 0, NULL);
    zval *file_data = zend_read_property(validator_ce, getThis(), "file_data", strlen("file_data"), 0, NULL);
    zval *cli_data = zend_read_property(validator_ce, getThis(), "cli_data", strlen("cli_data"), 0, NULL);

    /*
    bool is_success;
    zval result = Validator::doValidator(laws, get_data, post_data, file_data, cli_data, is_success);

    if (is_success)
    {
        zend_update_property(validator_ce, getThis(), "result", strlen("result"), &result);
    }

    zval_ptr_dtor(&result);
    */
}

PHP_METHOD(validator, getResult)
{
    zval *result = zend_read_property(validator_ce, getThis(), "result", strlen("result"), 0, NULL);

    RETURN_ZVAL(result, 1, 0);
}

PHP_METHOD(validator, setGetData)
{
    zval *getData;

    ZEND_PARSE_PARAMETERS_START(1, 1)
        Z_PARAM_ARRAY(getData)
    ZEND_PARSE_PARAMETERS_END();

    zend_update_property(validator_ce, getThis(), "get_data", strlen("get_data"), getData);
}

PHP_METHOD(validator, setPostData)
{
    zval *postData;

    ZEND_PARSE_PARAMETERS_START(1, 1)
        Z_PARAM_ARRAY(postData)
    ZEND_PARSE_PARAMETERS_END();

    zend_update_property(validator_ce, getThis(), "post_data", strlen("post_data"), postData);
}

PHP_METHOD(validator, setFileData)
{
    zval *fileData;

    ZEND_PARSE_PARAMETERS_START(1, 1)
        Z_PARAM_ARRAY(fileData)
    ZEND_PARSE_PARAMETERS_END();

    zend_update_property(validator_ce, getThis(), "file_data", strlen("file_data"), fileData);
}

PHP_METHOD(validator, setCliData)
{
    zval *cliData;

    ZEND_PARSE_PARAMETERS_START(1, 1)
        Z_PARAM_ARRAY(cliData)
    ZEND_PARSE_PARAMETERS_END();

    zend_update_property(validator_ce, getThis(), "cli_data", strlen("cli_data"), cliData);
}

zend_function_entry validator_methods[] = {
	PHP_ME(validator, __construct,         NULL,     ZEND_ACC_PUBLIC | ZEND_ACC_CTOR)
	PHP_ME(validator, doCheck,         NULL,     ZEND_ACC_PUBLIC)
	PHP_ME(validator, getResult,         NULL,     ZEND_ACC_PUBLIC)

	PHP_ME(validator, setGetData,         NULL,     ZEND_ACC_PUBLIC)
	PHP_ME(validator, setPostData,         NULL,     ZEND_ACC_PUBLIC)
	PHP_ME(validator, setFileData,         NULL,     ZEND_ACC_PUBLIC)
	PHP_ME(validator, setCliData,         NULL,     ZEND_ACC_PUBLIC)

	{NULL, NULL, NULL}
};

MYAPI_STARTUP_FUNCTION(validator)
{
    zend_class_entry ce;
    INIT_CLASS_ENTRY(ce, "myapi\\Validator", validator_methods);
    validator_ce = zend_register_internal_class(&ce);

    zend_declare_property_null(validator_ce, "laws", strlen("laws"), ZEND_ACC_PRIVATE);
    zend_declare_property_null(validator_ce, "get_data", strlen("get_data"), ZEND_ACC_PRIVATE);
    zend_declare_property_null(validator_ce, "post_data", strlen("post_data"), ZEND_ACC_PRIVATE);
    zend_declare_property_null(validator_ce, "file_data", strlen("file_data"), ZEND_ACC_PRIVATE);
    zend_declare_property_null(validator_ce, "cli_data", strlen("cli_data"), ZEND_ACC_PRIVATE);
    zend_declare_property_null(validator_ce, "result", strlen("result"), ZEND_ACC_PRIVATE);

    return SUCCESS;
}

