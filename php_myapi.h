/* myapi extension for PHP */
#include <string>
#include "error_code.h"

#ifndef PHP_MYAPI_H
# define PHP_MYAPI_H


# define phpext_myapi_ptr &myapi_module_entry

# define PHP_MYAPI_VERSION "0.1.0"

#define MYAPI_STARTUP_FUNCTION(module)      ZEND_MINIT_FUNCTION(module)
#define MYAPI_STARTUP(module)	 		  	ZEND_MODULE_STARTUP_N(module)(INIT_FUNC_ARGS_PASSTHRU)

#define CONSTRUCT "__construct"
#define API_AUTOLOAD "\\myapi\\api::autoLoad"

#define ROUTER_CLASS "\\myapi\\Router"
#define ROUTER_GET_API "getApi"
#define ROUTER_GET_MODULE "getModule"
#define ROUTER_GET_CONTROLLER "getController"
#define ROUTER_GET_ACTION "getAction"

#define RESPONSE_CLASS "\\myapi\\Response"
#define RESPONSE_OUTPUT "output"

/* For compatibility with older PHP versions */
#ifndef ZEND_PARSE_PARAMETERS_NONE
#define ZEND_PARSE_PARAMETERS_NONE() \
	ZEND_PARSE_PARAMETERS_START(0, 0) \
	ZEND_PARSE_PARAMETERS_END()
#endif

# if defined(ZTS) && defined(COMPILE_DL_MYAPI)
ZEND_TSRMLS_CACHE_EXTERN()
# endif

extern zend_module_entry myapi_module_entry;

#ifdef ZTS
#define MYAPI_G(v) TSRMG(myapi_globals_id, zend_myapi_globals *, v)
#else
#define MYAPI_G(v) (myapi_globals.v)
#endif

ZEND_BEGIN_MODULE_GLOBALS(myapi)
    //char* test_str;
	bool enable_error_handler;
	bool enable_exception_handler;
	bool enable_multi_enviroment;
	char* default_enviroment;
	char* language;
ZEND_END_MODULE_GLOBALS(myapi)

ZEND_EXTERN_MODULE_GLOBALS(myapi);

#endif	/* PHP_MYAPI_H */
