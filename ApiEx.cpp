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
#include "api.h"
#include "MyApiTool.h"
#include "ApiEx.h"
#include "RouterEx.h"
#include "RequestEx.h"
#include "ResponseEx.h"
#include "FilterEx.h"
#include <string>
#include <fstream>
#include <VersionEx.h>
#include "ControllerEx.h"

extern zend_class_entry *api_ce;

std::string Api::getApiPath()
{
    zval *val = zend_read_static_property(api_ce, "api_path", strlen("api_path"), 0 );

    if (val && Z_TYPE_P(val) == IS_STRING)
    {
        return Z_STRVAL_P(val);
    }

    return "";
}

std::string Api::getEnv()
{
    zval *val = zend_read_static_property(api_ce, "env", strlen("env"), 0 );

    if (val && Z_TYPE_P(val) == IS_STRING)
    {
        return Z_STRVAL_P(val);
    }

    return "";
}

std::string Api::getPhpSapi()
{
    zval *val = zend_read_static_property(api_ce, "php_sapi", strlen("php_sapi"), 0 );

    if (val && Z_TYPE_P(val) == IS_STRING)
    {
        return Z_STRVAL_P(val);
    }

    return "";
}

std::shared_ptr<zval> Api::import(std::string filename, bool isDelete)
{
    std::ifstream ifs(filename);

    if (!ifs.is_open())
    {
        //php_error_docref(NULL, E_WARNING, MYAPI_ERR_MSG(101), filename.c_str());
        return std::shared_ptr<zval>();
    }

    zval fileZval;
    ZVAL_STRING(&fileZval, filename.c_str());

    zend_op_array *arr = compile_filename(ZEND_INCLUDE_ONCE, &fileZval);
    zval_ptr_dtor(&fileZval);

    if (arr == NULL)
    {
        return std::shared_ptr<zval>();
    }

    zval *ret = (zval *) emalloc(sizeof(zval));
    std::shared_ptr<zval> retPtr(ret, [isDelete](zval *ptr){
        if (isDelete)
        {
            zval_ptr_dtor(ptr);
        }

        efree(ptr);
    });

    zend_execute(arr, ret);
    destroy_op_array(arr);
    efree(arr);

    return retPtr;
}

bool Api::run()
{
    std::string controller = Router::getInstance().getController();
    std::string module = Router::getInstance().getModule();
    std::string action = Router::getInstance().getAction();

    if (module == "")
    {
        Response::getInstance().setErrorResult(MYAPI_ERR(153));
        return false;
    }

    if (action == "")
    {
        Response::getInstance().setErrorResult(MYAPI_ERR(104));
        return false;
    }

    if (controller == "")
    {
        Response::getInstance().setErrorResult(MYAPI_ERR(105));
        return false;
    }

    bool beginFilterRet = Filter::getInstance().doBeginFilter();

    if (EG(exception))
    {
        return false;
    }

    if (!beginFilterRet)
    {
        bool isSetError = Response::getInstance().isError();

        if (!isSetError)
        {
            Response::getInstance().setErrorResult(MYAPI_ERR(103));
        }

        return false;
    }

    MyApiTool controllerObj(controller);

    if (!controllerObj.isCallable())
    {
        Response::getInstance().setErrorResult(MYAPI_ERR(106));
        return false;
    }

    controllerObj.callMethod(CONSTRUCT);
    zval conObj = controllerObj.getObject();
    bool isSuccess = Controller::getInstance().doValidator(conObj);

    if (!isSuccess)
    {
        return false;
    }

    zval *ret = NULL;
    std::shared_ptr<zval> controllerRetPtr = controllerObj.callMethod(action.c_str());

    if (!controllerRetPtr)
    {
        Response::getInstance().setErrorResult(MYAPI_ERR(104));
        return false;
    }

    if (EG(exception))
    {
        return false;
    }

    ret = controllerRetPtr.get();

    std::string versionClass = Router::getInstance().getVersionClass();
    std::shared_ptr<zval> versionRet;

    if (versionClass != "")
    {
        MyApiTool versionObj(versionClass);

        if (versionObj.isCallable())
        {
            zval action_name;
            ZVAL_STRING(&action_name, action.c_str());
            zval method_exists_params[] = {versionObj.getObject(), action_name};
            std::shared_ptr<zval> mePtr = MyApiTool::callFunction("method_exists", 2, method_exists_params);

            if (mePtr && Z_TYPE_P(mePtr.get()) == IS_TRUE)
            {
                Version::getInstance().setResult(ret);
                versionObj.callMethod(action.c_str());

                if (EG(exception))
                {
                    return false;
                }

                ret = Version::getInstance().getResult();
            }

            zval_ptr_dtor(&action_name);
        }
    }

    bool afterFilterRet = Filter::getInstance().doAfterFilter(ret);

    if (EG(exception))
    {
        return false;
    }

    if (!afterFilterRet)
    {
        bool isSetError = Response::getInstance().isError();

        if (!isSetError)
        {
            Response::getInstance().setErrorResult(MYAPI_ERR(107));
        }

        return false;
    }

    Response::getInstance().setResult(ret);

    return true;
}

void Api::init(char *path)
{
    zval *isInstantiated = zend_read_static_property(api_ce, "is_instantiated", strlen("is_instantiated"), 0);

    if (Z_TYPE_P(isInstantiated) == IS_TRUE)
    {
        MyApiTool::throwException(MYAPI_ERR(102));
        return;
    }

    zval autoload_function;

    std::shared_ptr<zval> sapiPtr = MyApiTool::getContant("PHP_SAPI", false);

    zend_update_static_property_string(api_ce, "php_sapi", strlen("php_sapi"), Z_STRVAL_P(sapiPtr.get()));

    std::string sapi_str = Z_STRVAL_P(sapiPtr.get());
    std::shared_ptr<zval> envRetPtr;
    zval *envPtr;

    if (sapi_str == "cli")
    {
        zval param;
        ZVAL_STRING(&param, "MYAPI_ENV");
        zval envParams[] = {param};
        envRetPtr = MyApiTool::callFunction("getenv", 1, envParams);
        zval_ptr_dtor(&param);
        envPtr = envRetPtr.get();
    }
    else
    {
        envPtr = MyApiTool::getZvalByHashTableEx(&EG(symbol_table), "_SERVER.MYAPI_ENV");
    }

    char *env = envPtr && Z_TYPE_P(envPtr) == IS_STRING ? Z_STRVAL_P(envPtr) : MYAPI_G(default_enviroment);

    zend_update_static_property_string(api_ce, "env", strlen("env"), env);

    zend_update_static_property_bool(api_ce, "is_instantiated", strlen("is_instantiated"), 1);
    zend_update_static_property_string(api_ce, "api_path", strlen("api_path"), path);

    std::shared_ptr<zval> autoloadFunPtr = MyApiTool::getZval(&autoload_function, [&autoload_function](){
        ZVAL_STRING(&autoload_function, API_AUTOLOAD);
    });

    zval params[] = {autoload_function};

    MyApiTool::callFunction("spl_autoload_register", 1, params);

    Request::getInstance().init();
    Router::getInstance().init();

    if (EG(exception))
    {
        return;
    }

    if (MYAPI_G(enable_exception_handler))
    {
        zval excepHandlerFunName;
        std::shared_ptr<zval> exceptionPtr = MyApiTool::getZval(&excepHandlerFunName, [&excepHandlerFunName](){
            ZVAL_STRING(&excepHandlerFunName, "\\myapi\\Api::exceptionHandler");
        });

        zval exceptionHandlerParams[] = {*exceptionPtr};
        MyApiTool::callFunction("set_exception_handler", 1, exceptionHandlerParams);
    }

    if (MYAPI_G(enable_error_handler))
    {
        zval errorHandlerFunName;
        std::shared_ptr<zval> errorHandlerPtr = MyApiTool::getZval(&errorHandlerFunName, [&errorHandlerFunName](){
            ZVAL_STRING(&errorHandlerFunName, "\\myapi\\Api::errorHandler");
        });

        zval errorHandlerParams[] = {*errorHandlerPtr};
        MyApiTool::callFunction("set_error_handler", 1, errorHandlerParams);
    }
}

bool Api::isInstantiated()
{
    zval *val = zend_read_static_property(api_ce, "is_instantiated", strlen("is_instantiated"), 0);

    if (Z_TYPE_P(val) == IS_TRUE)
    {
        return true;
    }

    return false;
}

Api Api::getInstance()
{
    static Api api;

    return api;
}
