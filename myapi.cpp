/* myapi extension for PHP */



extern "C" {
    #ifdef HAVE_CONFIG_H
    # include "config.h"
    #endif

    #include "php.h"
    #include "ext/standard/info.h"
    #include "php_myapi.h"
}

#include "api.h"
#include "controller.h"
#include "model.h"
#include "myapi_config.h"
#include "MyApiTool.h"
#include <vector>

/* For compatibility with older PHP versions */
#ifndef ZEND_PARSE_PARAMETERS_NONE
#define ZEND_PARSE_PARAMETERS_NONE() \
	ZEND_PARSE_PARAMETERS_START(0, 0) \
	ZEND_PARSE_PARAMETERS_END()
#endif

/* {{{ void myapi_test1()
 */
PHP_FUNCTION(myapi_test1)
{
	ZEND_PARSE_PARAMETERS_NONE();

	php_printf("The extension %s is loaded and working!\r\n", "myapi");
}
/* }}} */

/* {{{ string myapi_test2( [ string $var ] )
 */
PHP_FUNCTION(myapi_test2)
{
    zval ret;
    zval function_name;
    ZVAL_STRING(&function_name, "\\api\\controller\\MyTest::fun");
    call_user_function(NULL, NULL, &function_name, &ret, 0, NULL);
    zval_ptr_dtor(&function_name);

    if (Z_TYPE(ret) == IS_UNDEF)
    {
        zval_ptr_dtor(&ret);
        return;
    }

    zval_ptr_dtor(&ret);
}
/* }}}*/

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

PHP_MINIT_FUNCTION(myapi)
{
    MYAPI_STARTUP(api);
    MYAPI_STARTUP(controller);
    MYAPI_STARTUP(model);
    MYAPI_STARTUP(config);
    return SUCCESS;
}

PHP_MSHUTDOWN_FUNCTION(myapi)
{
    return SUCCESS;
}


/* {{{ PHP_MINFO_FUNCTION
 */
PHP_MINFO_FUNCTION(myapi)
{
	php_info_print_table_start();
	php_info_print_table_header(2, "myapi support", "enabled");
	php_info_print_table_end();
}
/* }}} */

/* {{{ arginfo
 */
ZEND_BEGIN_ARG_INFO(arginfo_myapi_test1, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_myapi_test2, 0)
	ZEND_ARG_INFO(0, str)
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
	NULL,							/* PHP_RSHUTDOWN - Request shutdown */
	PHP_MINFO(myapi),			/* PHP_MINFO - Module info */
	PHP_MYAPI_VERSION,		/* Version */
	STANDARD_MODULE_PROPERTIES
};
/* }}} */

#ifdef COMPILE_DL_MYAPI
# ifdef ZTS
ZEND_TSRMLS_CACHE_DEFINE()
# endif
ZEND_GET_MODULE(myapi)
#endif
