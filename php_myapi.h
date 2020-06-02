/* myapi extension for PHP */

#ifndef PHP_MYAPI_H
# define PHP_MYAPI_H


# define phpext_myapi_ptr &myapi_module_entry

# define PHP_MYAPI_VERSION "0.1.0"

#define MYAPI_STARTUP_FUNCTION(module)      ZEND_MINIT_FUNCTION(module)
#define MYAPI_STARTUP(module)	 		  	ZEND_MODULE_STARTUP_N(module)(INIT_FUNC_ARGS_PASSTHRU)

# if defined(ZTS) && defined(COMPILE_DL_MYAPI)
ZEND_TSRMLS_CACHE_EXTERN()
# endif

extern zend_module_entry myapi_module_entry;

#endif	/* PHP_MYAPI_H */
