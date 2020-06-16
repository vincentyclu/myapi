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
    std::shared_ptr<zval> getPtr = MyApiTool::getZvalByHashTable(&EG(symbol_table), "_GET", false);

    if (getPtr)
    {
        zend_update_static_property(request_ce, "get_data", strlen("get_data"), getPtr.get());
    }

    std::shared_ptr<zval> postPtr = MyApiTool::getZvalByHashTable(&EG(symbol_table), "_POST", false);

    if (postPtr)
    {
        zend_update_static_property(request_ce, "post_data", strlen("post_data"), postPtr.get());
    }

    std::shared_ptr<zval> serverPtr = MyApiTool::getZvalByHashTable(&EG(symbol_table), "_SERVER", false);

    if (serverPtr)
    {
        zend_update_static_property(request_ce, "server_data", strlen("server_data"), serverPtr.get());
    }

    std::shared_ptr<zval> filePtr = MyApiTool::getZvalByHashTable(&EG(symbol_table), "_FILES", false);

    if (filePtr)
    {
        zend_update_static_property(request_ce,  "file_data", strlen("file_data"), filePtr.get());
    }

    std::shared_ptr<zval> requestPtr = MyApiTool::getZvalByHashTable(&EG(symbol_table), "_REQUEST", false);

    if (requestPtr)
    {
        zend_update_static_property(request_ce,  "request_data", strlen("request_data"), requestPtr.get());
    }

    std::shared_ptr<zval> argvPtr = MyApiTool::getZvalByHashTable(&EG(symbol_table), "argv", false);

    if (argvPtr)
    {
        zend_update_static_property(request_ce,  "argv_data", strlen("argv_data"), argvPtr.get());
    }
}

std::shared_ptr<zval> Request::get(std::string key)
{
    std::shared_ptr<zval> retPtr;
    zval *get_data = zend_read_static_property(request_ce, "get_data", strlen("get_data"), 0);

    if (Z_TYPE_P(get_data) == IS_ARRAY)
    {
        retPtr = MyApiTool::getZvalByHashTable(Z_ARRVAL_P(get_data), key.c_str(), false);
    }

    return retPtr;
}

std::shared_ptr<zval> Request::post(std::string key)
{
    std::shared_ptr<zval> retPtr;
    zval *post_data = zend_read_static_property(request_ce, "post_data", strlen("post_data"), 0);

    if (Z_TYPE_P(post_data) == IS_ARRAY)
    {
        retPtr = MyApiTool::getZvalByHashTable(Z_ARRVAL_P(post_data), key.c_str(), false);
    }

    return retPtr;
}

std::shared_ptr<zval> Request::server(std::string key)
{
    std::shared_ptr<zval> retPtr;
    zval *server_data = zend_read_static_property(request_ce, "server_data", strlen("server_data"), 0);

    if (Z_TYPE_P(server_data) == IS_ARRAY)
    {
        retPtr = MyApiTool::getZvalByHashTable(Z_ARRVAL_P(server_data), key.c_str(), false);
    }

    return retPtr;
}

std::shared_ptr<zval> Request::file(std::string key)
{
    std::shared_ptr<zval> retPtr;
    zval *file_data = zend_read_static_property(request_ce, "file_data", strlen("file_data"), 0);

    if (Z_TYPE_P(file_data) == IS_ARRAY)
    {
        retPtr = MyApiTool::getZvalByHashTable(Z_ARRVAL_P(file_data), key.c_str(), false);
    }

    return retPtr;
}

std::shared_ptr<zval> Request::request(std::string key)
{
    std::shared_ptr<zval> retPtr;
    zval *request_data = zend_read_static_property(request_ce, "request_data", strlen("request_data"), 0);

    if (Z_TYPE_P(request_data) == IS_ARRAY)
    {
        retPtr = MyApiTool::getZvalByHashTable(Z_ARRVAL_P(request_data), key.c_str(), false);
    }

    return retPtr;
}

std::shared_ptr<zval> Request::argv(zend_long key)
{
    std::shared_ptr<zval> retPtr;
    zval *argv_data = zend_read_static_property(request_ce, "argv_data", strlen("argv_data"), 0);

    if (Z_TYPE_P(argv_data) == IS_ARRAY)
    {
        retPtr = MyApiTool::getZvalByHashTable(Z_ARRVAL_P(argv_data), key, false);
    }

    return retPtr;
}

Request Request::getInstance()
{
    static Request request;

    return request;
}
