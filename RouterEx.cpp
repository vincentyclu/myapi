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
#include "RouterEx.h"

extern zend_class_entry *router_ce;

std::string Router::getApi()
{
    zval *val = zend_read_static_property(router_ce, "api", strlen("api"), 0);

    if (val && Z_TYPE_P(val) == IS_STRING)
    {
        return Z_STRVAL_P(val);
    }

    return "";
}

std::string Router::getModule()
{
    zval *val = zend_read_static_property(router_ce, "module", strlen("module"), 0);

    if (val && Z_TYPE_P(val) == IS_STRING)
    {
        return Z_STRVAL_P(val);
    }

    return "";
}

std::string Router::getAction()
{
    zval *val = zend_read_static_property(router_ce, "action", strlen("action"), 0);

    if (val && Z_TYPE_P(val) == IS_STRING)
    {
        return Z_STRVAL_P(val);
    }

    return "";
}

std::string Router::getController()
{
    zval *api = zend_read_static_property(router_ce, "api", strlen("api"), 0);
    zval *module = zend_read_static_property(router_ce, "module", strlen("module"), 0);

    if (!api || !module)
    {
        return "";
    }

    std::string controller = std::string("\\") + Z_STRVAL_P(api) + "\\controller\\" + Z_STRVAL_P(module);

    return controller;
}

std::string Router::getVersionClass()
{
    zval *api = zend_read_static_property(router_ce, "api", strlen("api"), 0);
    zval *module = zend_read_static_property(router_ce, "module", strlen("module"), 0);

    if (!api || !module)
    {
        return "";
    }

    std::string versionClass = std::string("\\") + Z_STRVAL_P(api) + "\\version\\" + Z_STRVAL_P(module);

    return versionClass;
}

void Router::init()
{
    std::shared_ptr<zval> sapiPtr = MyApiTool::getContant("PHP_SAPI", false);

    if (sapiPtr)
    {
        zend_update_static_property(router_ce, "php_sapi", strlen("php_sapi"), sapiPtr.get());
    }

    zval* apiPtr;
    zval* modulePtr;
    zval* actionPtr;

    if (std::string(Z_STRVAL_P(sapiPtr.get())) == "cli")
    {
        zval* argcPtr = MyApiTool::getZvalByHashTable(&EG(symbol_table), "argc");
        zval* argvPtr = MyApiTool::getZvalByHashTable(&EG(symbol_table), "argv");

        if (Z_LVAL_P(argcPtr) == 3)
        {
            modulePtr = MyApiTool::getZvalByHashTable(Z_ARRVAL_P(argvPtr), (zend_long) 1);
            actionPtr = MyApiTool::getZvalByHashTable(Z_ARRVAL_P(argvPtr), (zend_long) 2);
        }
        else if (Z_LVAL_P(argcPtr) >= 4)
        {
            apiPtr = MyApiTool::getZvalByHashTable(Z_ARRVAL_P(argvPtr), (zend_long) 1);
            modulePtr = MyApiTool::getZvalByHashTable(Z_ARRVAL_P(argvPtr), (zend_long) 2);
            actionPtr = MyApiTool::getZvalByHashTable(Z_ARRVAL_P(argvPtr), (zend_long) 3);
        }
        else
        {
            MyApiTool::throwException(MYAPI_ERR(150));
            return;
        }
    }
    else
    {
        zval* getPtr = MyApiTool::getZvalByHashTable(&EG(symbol_table), "_GET");
        apiPtr = MyApiTool::getZvalByHashTable(Z_ARRVAL_P(getPtr), "api");
        modulePtr = MyApiTool::getZvalByHashTable(Z_ARRVAL_P(getPtr), "module");
        actionPtr = MyApiTool::getZvalByHashTable(Z_ARRVAL_P(getPtr), "action");
    }

    if (!apiPtr)
    {
        zend_update_static_property_string(router_ce, "api", strlen("api"), "api");
    }
    else
    {
        zend_update_static_property(router_ce, "api", strlen("api"), apiPtr);
    }

    if (modulePtr)
    {
        zend_update_static_property(router_ce, "module", strlen("module"), modulePtr);
    }
    else
    {
        MyApiTool::throwException(MYAPI_ERR(151));
        return;
    }

    if (actionPtr)
    {
        zend_update_static_property(router_ce, "action", strlen("action"), actionPtr);
    }
    else
    {
        MyApiTool::throwException(MYAPI_ERR(152));
        return;
    }
}

Router Router::getInstance()
{
    static Router router;

    return router;
}
