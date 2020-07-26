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
#include "RequestEx.h"

extern zend_class_entry *request_ce;

void Request::init()
{
    zend_bool jit_init = PG(auto_globals_jit);
    if (jit_init)
    {
        zend_is_auto_global_str((char *)"_SERVER", strlen("_SERVER"));
        zend_is_auto_global_str((char *)"_REQUEST", strlen("_REQUEST"));
        zend_is_auto_global_str((char *)"_ENV", strlen("_ENV"));
    }

    zval* getPtr = MyApiTool::getZvalByHashTable(&EG(symbol_table), "_GET");

    if (getPtr)
    {
        zend_update_static_property(request_ce, "get_data", strlen("get_data"), getPtr);
    }

    zval* postPtr = MyApiTool::getZvalByHashTable(&EG(symbol_table), "_POST");

    if (postPtr)
    {
        zend_update_static_property(request_ce, "post_data", strlen("post_data"), postPtr);
    }

    zval* serverPtr = MyApiTool::getZvalByHashTable(&EG(symbol_table), "_SERVER");

    if (serverPtr)
    {
        zend_update_static_property(request_ce, "server_data", strlen("server_data"), serverPtr);
    }

    zval* filePtr = MyApiTool::getZvalByHashTable(&EG(symbol_table), "_FILES");

    if (filePtr)
    {
        zend_update_static_property(request_ce,  "file_data", strlen("file_data"), filePtr);
    }

    zval* requestPtr = MyApiTool::getZvalByHashTable(&EG(symbol_table), "_REQUEST");

    if (requestPtr)
    {
        zend_update_static_property(request_ce,  "request_data", strlen("request_data"), requestPtr);
    }

    zval* argvPtr = MyApiTool::getZvalByHashTable(&EG(symbol_table), "argv");

    if (argvPtr)
    {
        zend_update_static_property(request_ce,  "argv_data", strlen("argv_data"), argvPtr);
    }
}

zval* Request::get(std::string key)
{
    zval *retPtr = NULL;
    zval *get_data = zend_read_static_property(request_ce, "get_data", strlen("get_data"), 0);

    if (Z_TYPE_P(get_data) == IS_ARRAY)
    {
        retPtr = MyApiTool::getZvalByHashTable(Z_ARRVAL_P(get_data), key.c_str());
    }

    return retPtr;
}

zval* Request::post(std::string key)
{
    zval *retPtr = NULL;
    zval *post_data = zend_read_static_property(request_ce, "post_data", strlen("post_data"), 0);

    if (Z_TYPE_P(post_data) == IS_ARRAY)
    {
        retPtr = MyApiTool::getZvalByHashTable(Z_ARRVAL_P(post_data), key.c_str());
    }

    return retPtr;
}

zval* Request::server(std::string key)
{
    zval *retPtr = NULL;
    zval *server_data = zend_read_static_property(request_ce, "server_data", strlen("server_data"), 0);

    if (Z_TYPE_P(server_data) == IS_ARRAY)
    {
        retPtr = MyApiTool::getZvalByHashTable(Z_ARRVAL_P(server_data), key.c_str());
    }

    return retPtr;
}

zval* Request::file(std::string key)
{
    zval *retPtr = NULL;
    zval *file_data = zend_read_static_property(request_ce, "file_data", strlen("file_data"), 0);

    if (Z_TYPE_P(file_data) == IS_ARRAY)
    {
        retPtr = MyApiTool::getZvalByHashTable(Z_ARRVAL_P(file_data), key.c_str());
    }

    return retPtr;
}

zval* Request::request(std::string key)
{
    zval *retPtr = NULL;
    zval *request_data = zend_read_static_property(request_ce, "request_data", strlen("request_data"), 0);

    if (Z_TYPE_P(request_data) == IS_ARRAY)
    {
        retPtr = MyApiTool::getZvalByHashTable(Z_ARRVAL_P(request_data), key.c_str());
    }

    return retPtr;
}

zval* Request::argv(zend_long key)
{
    zval *retPtr = NULL;
    zval *argv_data = zend_read_static_property(request_ce, "argv_data", strlen("argv_data"), 0);

    if (Z_TYPE_P(argv_data) == IS_ARRAY)
    {
        retPtr = MyApiTool::getZvalByHashTable(Z_ARRVAL_P(argv_data), key);
    }

    return retPtr;
}

Request Request::getInstance()
{
    static Request request;

    return request;
}
