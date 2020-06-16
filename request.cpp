extern "C" {
    #ifdef HAVE_CONFIG_H
    # include "config.h"
    #endif

    #include "php.h"
    #include "ext/standard/info.h"
    #include "ext/standard/php_var.h"
}
#include "php_myapi.h"
#include "request.h"
#include "MyApiTool.h"
#include <vector>
#include <string>
#include "RequestEx.h"

zend_class_entry* request_ce;

PHP_METHOD(request, __construct)
{

}

PHP_METHOD(request, get)
{
    char *key;
    size_t len;

    ZEND_PARSE_PARAMETERS_START(1, 1)
        Z_PARAM_STRING(key, len);
    ZEND_PARSE_PARAMETERS_END();

    std::shared_ptr<zval> get_ptr = Request::getInstance().get(key);

    if (!get_ptr)
    {
        RETURN_NULL();
    }

    RETURN_ZVAL(get_ptr.get(), 1, 0);
}

PHP_METHOD(request, post)
{
    char *key;
    size_t len;

    ZEND_PARSE_PARAMETERS_START(1, 1)
        Z_PARAM_STRING(key, len);
    ZEND_PARSE_PARAMETERS_END();

    std::shared_ptr<zval> post_ptr = Request::getInstance().post(key);

    if (!post_ptr)
    {
        RETURN_NULL();
    }

    RETURN_ZVAL(post_ptr.get(), 1, 0);
}

PHP_METHOD(request, server)
{
    char *key;
    size_t len;

    ZEND_PARSE_PARAMETERS_START(1, 1)
        Z_PARAM_STRING(key, len);
    ZEND_PARSE_PARAMETERS_END();

    std::shared_ptr<zval> server_ptr = Request::getInstance().server(key);

    if (!server_ptr)
    {
        RETURN_NULL();
    }

    RETURN_ZVAL(server_ptr.get(), 1, 0);
}

PHP_METHOD(request, file)
{
    char *key;
    size_t len;

    ZEND_PARSE_PARAMETERS_START(1, 1)
        Z_PARAM_STRING(key, len);
    ZEND_PARSE_PARAMETERS_END();

    std::shared_ptr<zval> file_ptr = Request::getInstance().file(key);

    if (!file_ptr)
    {
        RETURN_NULL();
    }

    RETURN_ZVAL(file_ptr.get(), 1, 0);
}

PHP_METHOD(request, request)
{
    char *key;
    size_t len;

    ZEND_PARSE_PARAMETERS_START(1, 1)
        Z_PARAM_STRING(key, len);
    ZEND_PARSE_PARAMETERS_END();

    std::shared_ptr<zval> request_ptr = Request::getInstance().request(key);

    if (!request_ptr)
    {
        RETURN_NULL();
    }

    RETURN_ZVAL(request_ptr.get(), 1, 0);
}

PHP_METHOD(request, argv)
{
    zend_long key;

    ZEND_PARSE_PARAMETERS_START(1, 1)
        Z_PARAM_LONG(key);
    ZEND_PARSE_PARAMETERS_END();

    std::shared_ptr<zval> argv_ptr = Request::getInstance().argv(key);

    if (!argv_ptr)
    {
        RETURN_NULL();
    }

    RETURN_ZVAL(argv_ptr.get(), 1, 0);
}

zend_function_entry request_methods[] = {
	PHP_ME(request, __construct,         NULL,     ZEND_ACC_PUBLIC | ZEND_ACC_CTOR)
	PHP_ME(request, get,         NULL,     ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
	PHP_ME(request, post,         NULL,     ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
	PHP_ME(request, server,         NULL,     ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
	PHP_ME(request, file,         NULL,     ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
	PHP_ME(request, request,         NULL,     ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
	PHP_ME(request, argv,         NULL,     ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)

	{NULL, NULL, NULL}
};

MYAPI_STARTUP_FUNCTION(request)
{
    zend_class_entry ce;
    INIT_CLASS_ENTRY(ce, "myapi\\Request", request_methods);
    request_ce = zend_register_internal_class(&ce);

    zend_declare_property_null(request_ce, "get_data", strlen("get_data"), ZEND_ACC_PRIVATE | ZEND_ACC_STATIC);
    zend_declare_property_null(request_ce, "post_data", strlen("post_data"), ZEND_ACC_PRIVATE | ZEND_ACC_STATIC);
    zend_declare_property_null(request_ce, "server_data", strlen("server_data"), ZEND_ACC_PRIVATE | ZEND_ACC_STATIC);
    zend_declare_property_null(request_ce, "file_data", strlen("file_data"), ZEND_ACC_PRIVATE | ZEND_ACC_STATIC);
    zend_declare_property_null(request_ce, "request_data", strlen("request_data"), ZEND_ACC_PRIVATE | ZEND_ACC_STATIC);
    zend_declare_property_null(request_ce, "argv_data", strlen("argv_data"), ZEND_ACC_PRIVATE | ZEND_ACC_STATIC);

    return SUCCESS;
}

