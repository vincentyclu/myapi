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
#include "ApiEx.h"

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

    if (Z_TYPE_P(api) != IS_STRING || Z_TYPE_P(module) != IS_STRING)
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

    zval* apiPtr = NULL;
    zval* modulePtr = NULL;
    zval* actionPtr = NULL;

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
    }
    else
    {
        zval* getPtr = MyApiTool::getZvalByHashTable(&EG(symbol_table), "_GET");
        apiPtr = MyApiTool::getZvalByHashTable(Z_ARRVAL_P(getPtr), "api");
        modulePtr = MyApiTool::getZvalByHashTable(Z_ARRVAL_P(getPtr), "m");
        actionPtr = MyApiTool::getZvalByHashTable(Z_ARRVAL_P(getPtr), "a");
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

    if (actionPtr)
    {
        zend_update_static_property(router_ce, "action", strlen("action"), actionPtr);
    }

    zval* pathInfo = MyApiTool::getZvalByHashTableEx(&EG(symbol_table), "_SERVER.PATH_INFO");

    if (pathInfo && Z_TYPE_P(pathInfo) == IS_STRING && Z_STRLEN_P(pathInfo) > 0)
    {
        zend_update_static_property_string(router_ce, "path_info", strlen("path_info"), Z_STRVAL_P(pathInfo));
    }
    else
    {
        std::string pathInfoStr;

        if (apiPtr && modulePtr && actionPtr)
        {
            pathInfoStr = std::string("/") + Z_STRVAL_P(apiPtr) + "/" + Z_STRVAL_P(modulePtr) + "/" + Z_STRVAL_P(actionPtr);
        }
        else if (!apiPtr && modulePtr && actionPtr)
        {
            pathInfoStr = std::string("/") + Z_STRVAL_P(modulePtr) + "/" + Z_STRVAL_P(actionPtr);
        }
        else if (!apiPtr && !modulePtr && !actionPtr)
        {
            pathInfoStr = "/";
        }
        else
        {
            pathInfoStr = "/404";
        }

        zend_update_static_property_string(router_ce, "path_info", strlen("path_info"), pathInfoStr.c_str());
    }

    zval routerObj;
    object_init_ex(&routerObj, router_ce);
    MyApiTool::callMethodWithObject(routerObj, "__construct");
    zend_update_static_property(router_ce, "router_obj", strlen("router_obj"), &routerObj);

    std::string apiPath = Api::getInstance().getApiPath();
    std::shared_ptr<zval> bootstrapPtr = Api::import(apiPath + "/config/bootstrap.php");
    zval params[] = {routerObj};
    MyApiTool::callClosure(bootstrapPtr.get(), 1, params);
    zval_ptr_dtor(&routerObj);
}

bool Router::checkRouter()
{
    std::string api = getApi();
    std::string module = getModule();
    std::string action = getAction();

    if (api == "" || module == "" || action == "")
    {
        return false;
    }

    return true;
}

Router Router::getInstance()
{
    static Router router;

    return router;
}
