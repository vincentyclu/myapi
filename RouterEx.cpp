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
    zval *val = zend_read_static_property(router_ce, "controller", strlen("controller"), 0);

    if (val && Z_TYPE_P(val) == IS_STRING)
    {
        return Z_STRVAL_P(val);
    }

    return "";
}

std::string Router::getVersionClass()
{
    zval *val = zend_read_static_property(router_ce, "version_class", strlen("version_class"), 0);

    if (val && Z_TYPE_P(val) == IS_STRING)
    {
        return Z_STRVAL_P(val);
    }

    return "";
}

void Router::init()
{
    std::shared_ptr<zval> sapiPtr = MyApiTool::getContant("PHP_SAPI", false);

    if (sapiPtr)
    {
        zend_update_static_property(router_ce, "php_sapi", strlen("php_sapi"), sapiPtr.get());
    }

    std::shared_ptr<zval> apiPtr;
    std::shared_ptr<zval> modulePtr;
    std::shared_ptr<zval> actionPtr;
    zval apiZval;

    if (std::string(Z_STRVAL_P(sapiPtr.get())) == "cli")
    {
        std::shared_ptr<zval> argcPtr = MyApiTool::getZvalByHashTable(&EG(symbol_table), "argc", false);
        std::shared_ptr<zval> argvPtr = MyApiTool::getZvalByHashTable(&EG(symbol_table), "argv", false);

        if (Z_LVAL_P(argcPtr.get()) == 3)
        {
            modulePtr = MyApiTool::getZvalByHashTable(Z_ARRVAL_P(argvPtr.get()), (zend_long) 1, false);
            actionPtr = MyApiTool::getZvalByHashTable(Z_ARRVAL_P(argvPtr.get()), (zend_long) 2, false);
        }
        else if (Z_LVAL_P(argcPtr.get()) >= 4)
        {
            apiPtr = MyApiTool::getZvalByHashTable(Z_ARRVAL_P(argvPtr.get()), (zend_long) 1, false);
            modulePtr = MyApiTool::getZvalByHashTable(Z_ARRVAL_P(argvPtr.get()), (zend_long) 2, false);
            actionPtr = MyApiTool::getZvalByHashTable(Z_ARRVAL_P(argvPtr.get()), (zend_long) 3, false);
        }
        else
        {
            MyApiTool::throwException("router error", 10);
            return;
        }
    }
    else
    {
        std::shared_ptr<zval> getPtr = MyApiTool::getZvalByHashTable(&EG(symbol_table), "_GET", false);
        apiPtr = MyApiTool::getZvalByHashTable(Z_ARRVAL_P(getPtr.get()), "api", false);
        modulePtr = MyApiTool::getZvalByHashTable(Z_ARRVAL_P(getPtr.get()), "module", false);
        actionPtr = MyApiTool::getZvalByHashTable(Z_ARRVAL_P(getPtr.get()), "action", false);
    }

    if (!apiPtr)
    {
        apiPtr = MyApiTool::getZval(&apiZval, [&apiZval](){
                ZVAL_STRING(&apiZval, "api");
            });
    }

    zend_update_static_property(router_ce, "api", strlen("api"), apiPtr.get());

    if (modulePtr)
    {
        zend_update_static_property(router_ce, "module", strlen("module"), modulePtr.get());
    }
    else
    {
        MyApiTool::throwException("module cannot be empty", 10);
        return;
    }

    if (actionPtr)
    {
        zend_update_static_property(router_ce, "action", strlen("action"), actionPtr.get());
    }
    else
    {
        MyApiTool::throwException("action cannot be empty", 10);
        return;
    }

    std::string moduleStr = Z_STRVAL_P(modulePtr.get());
    //moduleStr[0] = toupper(moduleStr[0]);
    std::string controller = std::string("\\") + Z_STRVAL_P(apiPtr.get()) + "\\controller\\" + moduleStr;
    std::string versionClass = std::string("\\") + Z_STRVAL_P(apiPtr.get()) + "\\version\\" + moduleStr;

    zend_update_static_property_string(router_ce, "controller", strlen("controller"), controller.c_str());
    zend_update_static_property_string(router_ce, "version_class", strlen("version_class"), versionClass.c_str());
}

Router Router::getInstance()
{
    static Router router;

    return router;
}
