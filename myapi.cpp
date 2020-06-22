/* myapi extension for PHP */



extern "C" {
    #ifdef HAVE_CONFIG_H
    # include "config.h"
    #endif

    #include "php.h"
    #include "ext/standard/info.h"
    #include "ext/standard/php_var.h"
    #include "ext/standard/file.h"
    #include "ext/date/php_date.h"
    #include "ext/standard/php_string.h"
    #include "zend_compile.h"
    #include "zend_closures.h"
}
#include "php_myapi.h"
#include "api.h"
#include "controller.h"
#include "model.h"
#include "myapi_config.h"
#include "request.h"
#include "response.h"
#include "router.h"
#include "validator.h"
#include "exception.h"
#include "filter.h"
#include "version.h"
#include "log.h"
#include "MyApiTool.h"
#include "zend_exceptions.h"
#include <memory>
#include "ApiEx.h"
#include "ConfigEx.h"
#include "LogEx.h"
#include "FilterEx.h"
#include "SqlParser.h"
#include "orm.h"
#include <stdlib.h>
#include "ResponseEx.h"


ZEND_INI_MH(abc)
{
    //return FAILURE;
    //php_printf("%s", ZSTR_VAL(new_value));
    return SUCCESS;
}

PHP_INI_BEGIN()
	STD_PHP_INI_ENTRY("myapi.enable_error_handler",    "1",  PHP_INI_ALL, OnUpdateBool, enable_error_handler, zend_myapi_globals, myapi_globals)
	STD_PHP_INI_ENTRY("myapi.enable_exception_handler",   "0",  PHP_INI_ALL, OnUpdateBool, enable_exception_handler, zend_myapi_globals, myapi_globals)
	STD_PHP_INI_ENTRY("myapi.enable_multi_enviroment",   "0",  PHP_INI_ALL, OnUpdateBool, enable_multi_enviroment, zend_myapi_globals, myapi_globals)
	STD_PHP_INI_ENTRY("myapi.default_enviroment",   "dev",  PHP_INI_ALL, OnUpdateString, default_enviroment, zend_myapi_globals, myapi_globals)
	STD_PHP_INI_ENTRY("myapi.language",   "zh",  PHP_INI_ALL, OnUpdateString, language, zend_myapi_globals, myapi_globals)
	//PHP_INI_ENTRY("myapi.test_var",     "hello", PHP_INI_ALL, abc)
PHP_INI_END();

ZEND_DECLARE_MODULE_GLOBALS(myapi);

/* {{{ void myapi_test1()
 */

 struct zend_closure;

PHP_FUNCTION(myapi_test1)
{
    zval *val;
    ZEND_PARSE_PARAMETERS_START(1, 1)
        Z_PARAM_ARRAY(val)
    ZEND_PARSE_PARAMETERS_END();

    std::string str = Response::getInstance().getXmlString(val);

    RETURN_STRING(str.c_str());
}
/* }}} */

/* {{{ string myapi_test2( [ string $var ] )
 */

 extern zend_class_entry* config_ce;

PHP_FUNCTION(myapi_test2)
{
    zval *val = MyApiTool::getZvalByHashTableEx(&EG(symbol_table), "");

    if (!val)
    {
        RETURN_NULL();
    }

    RETURN_ZVAL(val, 1, 0);
}
/* }}}*/

PHP_GINIT_FUNCTION(myapi)
{
	memset(myapi_globals, 0, sizeof(*myapi_globals));
}

/* {{{ PHP_RINIT_FUNCTION
 */
PHP_RINIT_FUNCTION(myapi)
{
#if defined(ZTS) && defined(COMPILE_DL_MYAPI)
	ZEND_TSRMLS_CACHE_UPDATE();
#endif
	return SUCCESS;
}
/* }}} */

PHP_RSHUTDOWN_FUNCTION(myapi)
{


    return SUCCESS;
}

PHP_MINIT_FUNCTION(myapi)
{
    REGISTER_INI_ENTRIES();

    MYAPI_STARTUP(api);
    MYAPI_STARTUP(controller);
    MYAPI_STARTUP(model);
    MYAPI_STARTUP(config);
    MYAPI_STARTUP(request);
    MYAPI_STARTUP(response);
    MYAPI_STARTUP(router);
    MYAPI_STARTUP(validator);
    MYAPI_STARTUP(exception);
    MYAPI_STARTUP(filter);
    MYAPI_STARTUP(version);
    MYAPI_STARTUP(log);
    MYAPI_STARTUP(orm);
    return SUCCESS;
}

PHP_MSHUTDOWN_FUNCTION(myapi)
{
    UNREGISTER_INI_ENTRIES();

    return SUCCESS;
}


/* {{{ PHP_MINFO_FUNCTION
 */
PHP_MINFO_FUNCTION(myapi)
{
	php_info_print_table_start();
	php_info_print_table_header(2, "myapi support", "enabled");
	php_info_print_table_row(2, "Version", PHP_MYAPI_VERSION);
	php_info_print_table_end();

	DISPLAY_INI_ENTRIES();
}
/* }}} */

/* {{{ arginfo
 */
ZEND_BEGIN_ARG_INFO(arginfo_myapi_test1, 0)
    //ZEND_ARG_TYPE_INFO(0, test_str, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_myapi_test2, 0)
	//ZEND_ARG_OBJ_INFO(0, filterObj, Test, 0)
ZEND_END_ARG_INFO()
/* }}} */

/* {{{ myapi_functions[]
 */
static const zend_function_entry myapi_functions[] = {
	PHP_FE(myapi_test1,		arginfo_myapi_test1)
	PHP_FE(myapi_test2,		arginfo_myapi_test2)
	PHP_FE_END
};
/* }}} */

/* {{{ myapi_module_entry
 */
zend_module_entry myapi_module_entry = {
	STANDARD_MODULE_HEADER,
	"myapi",					/* Extension name */
	myapi_functions,			/* zend_function_entry */
	PHP_MINIT(myapi),							/* PHP_MINIT - Module initialization */
	PHP_MSHUTDOWN(myapi),							/* PHP_MSHUTDOWN - Module shutdown */
	PHP_RINIT(myapi),			/* PHP_RINIT - Request initialization */
	PHP_RSHUTDOWN(myapi),							/* PHP_RSHUTDOWN - Request shutdown */
	PHP_MINFO(myapi),			/* PHP_MINFO - Module info */
	PHP_MYAPI_VERSION,		/* Version */
	PHP_MODULE_GLOBALS(myapi),
	PHP_GINIT(myapi),
	NULL,
	NULL,
	STANDARD_MODULE_PROPERTIES_EX
};
/* }}} */

#ifdef COMPILE_DL_MYAPI
# ifdef ZTS
ZEND_TSRMLS_CACHE_DEFINE()
# endif
ZEND_GET_MODULE(myapi)
#endif
