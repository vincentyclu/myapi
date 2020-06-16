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
#include <vector>
#include <fstream>
#include <VersionEx.h>

zend_class_entry *api_ce;

PHP_METHOD(api, __construct)
{
    char *path = NULL;
    size_t var_len = 0;
    zval autoload_function;

    ZEND_PARSE_PARAMETERS_START(1, 1)
        Z_PARAM_STRING(path, var_len)
    ZEND_PARSE_PARAMETERS_END();

    zval *isInstantiated = zend_read_static_property(api_ce, "is_instantiated", strlen("is_instantiated"), 0);

    if (Z_TYPE_P(isInstantiated) == IS_TRUE)
    {
        MyApiTool::throwException("API CANNOT BE INSTANTIATED TWICE", 10);
        return;
    }

    std::shared_ptr<zval> sapiPtr = MyApiTool::getContant("PHP_SAPI", false);

    zend_update_static_property_string(api_ce, "php_sapi", strlen("php_sapi"), Z_STRVAL_P(sapiPtr.get()));

    std::string sapi_str = Z_STRVAL_P(sapiPtr.get());
    std::shared_ptr<zval> envPtr;

    if (sapi_str == "cli")
    {
        zval param;
        ZVAL_STRING(&param, "MYAPI_ENV");
        zval envParams[] = {param};
        envPtr = MyApiTool::callFunction("getenv", 1, envParams);
        zval_ptr_dtor(&param);
    }
    else
    {
        envPtr = MyApiTool::getZvalByHashTableEx(&EG(symbol_table), "_SERVER.MYAPI_ENV", false);

    }

    char *env = envPtr ? Z_STRVAL_P(envPtr.get()) : MYAPI_G(default_enviroment);

    zend_update_static_property_string(api_ce, "env", strlen("env"), env);

    zend_update_static_property_bool(api_ce, "is_instantiated", strlen("is_instantiated"), 1);
    zend_update_static_property_string(api_ce, "api_path", strlen("api_path"), path);

    std::shared_ptr<zval> autoloadFunPtr = MyApiTool::getZval(&autoload_function, [&autoload_function](){
        ZVAL_STRING(&autoload_function, API_AUTOLOAD);
    });

    zval params[] = {autoload_function};

    MyApiTool::callFunction("spl_autoload_register", 1, params);

    Router::getInstance().init();
    Request::getInstance().init();

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

PHP_METHOD(api, import)
{
    zval* filename;
    zval ret;
    ZEND_PARSE_PARAMETERS_START(1, 1)
        Z_PARAM_ZVAL(filename)
    ZEND_PARSE_PARAMETERS_END();

    std::ifstream ifs(Z_STRVAL_P(filename));

    if (!ifs.is_open())
    {
        php_error_docref(NULL, E_WARNING, "%s, FILE NOT EXISTING", Z_STRVAL_P(filename));
        return;
    }

    zend_op_array *arr = compile_filename(ZEND_INCLUDE_ONCE, filename);

    if (arr == NULL)
    {
        return;
    }

    zend_execute(arr, return_value);
    destroy_op_array(arr);
    efree(arr);

}

PHP_METHOD(api, autoLoad)
{

    zval* filename;
    zval compile_filename_path;
    zval *api_path = zend_read_static_property(api_ce, "api_path", strlen("api_path"), 0);

    ZEND_PARSE_PARAMETERS_START(1, 1)
        Z_PARAM_ZVAL(filename)
    ZEND_PARSE_PARAMETERS_END();

    std::string include_file_path = Z_STRVAL_P(api_path);
    std::string filename_str = Z_STRVAL_P(filename);

    for (auto &s : filename_str)
    {
        if (s == '\\')
        {
            s = '/';
        }
    }

    include_file_path = include_file_path + "/" + filename_str + ".php";

    std::ifstream ifs(include_file_path);

    if (!ifs.is_open())
    {
        include_file_path += " FILE NOT EXISTING";
        php_error_docref(NULL, E_WARNING, "%s", include_file_path.c_str());
        return;
    }

    ZVAL_STRING(&compile_filename_path, include_file_path.c_str());

    zend_op_array *arr = compile_filename(ZEND_INCLUDE_ONCE, &compile_filename_path);

    zval_ptr_dtor(&compile_filename_path);

    if (arr == NULL)
    {
        return;
    }

    zend_execute(arr, return_value);
    destroy_op_array(arr);
    efree(arr);


}

PHP_METHOD(api, run)
{
    bool beginFilterRet = Filter::getInstance().doBeginFilter();

    if (!beginFilterRet)
    {
        std::string beginFilterError = Response::getInstance().getErrorResult("begin filter error", 10);
        RETURN_STRING(beginFilterError.c_str());
    }

    std::string controller = Router::getInstance().getController();
    std::string action = Router::getInstance().getAction();

    if (action == "")
    {
        MyApiTool::throwException("ACTION NOT FOUND", 10);
        return;
    }

    if (controller == "")
    {
        MyApiTool::throwException("CONTROLLER NOT FOUND", 10);
        return;
    }

    MyApiTool controllerObj(controller);

    if (!controllerObj.isCallable())
    {
        MyApiTool::throwException("CONTROLLER CANNOT BE CALLABLE", 10);
        RETURN_NULL();
    }

    controllerObj.callMethod(CONSTRUCT);
    std::shared_ptr<zval> validatorPtr = controllerObj.callMethod("doValidator");
    zval *ret = NULL;

    std::shared_ptr<zval> controllerRetPtr;

    if (Z_TYPE_P(validatorPtr.get()) != IS_TRUE)
    {
        ret = validatorPtr.get();
    }
    else
    {
        controllerRetPtr = controllerObj.callMethod(action.c_str());

        if (!controllerRetPtr || Z_TYPE(*controllerRetPtr) == IS_UNDEF)
        {
            return;
        }

        ret = controllerRetPtr.get();
    }

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
                ret = Version::getInstance().getResult();
            }

            zval_ptr_dtor(&action_name);
        }
    }

    bool afterFilterRet = Filter::getInstance().doAfterFilter(ret);

    if (!afterFilterRet)
    {
        std::string afterFilterError = Response::getInstance().getErrorResult("after filter error", 10);
        RETURN_STRING(afterFilterError.c_str());;
    }

    std::string result = Response::getInstance().getJsonString(ret);

    if (result == "")
    {
        RETURN_ZVAL(ret, 1, 0);
    }
    else
    {
        RETURN_STRING(result.c_str());
    }

}

PHP_METHOD(api, exceptionHandler)
{
    zval *filterObj;
    ZEND_PARSE_PARAMETERS_START(1, 1)
        Z_PARAM_OBJECT(filterObj)
    ZEND_PARSE_PARAMETERS_END();

    std::shared_ptr<zval> msgPtr = MyApiTool::callMethodWithObject(*filterObj, "getMessage");
    std::shared_ptr<zval> codePtr = MyApiTool::callMethodWithObject(*filterObj, "getCode");

    zval ret;
    std::shared_ptr<zval> retPtr = MyApiTool::getZval(&ret, [&ret, &msgPtr, &codePtr](){
        array_init(&ret);
        add_assoc_zval(&ret, "message", msgPtr.get());
        add_assoc_zval(&ret, "code", codePtr.get());
    });

    smart_str buf = {0};
    php_json_encode(&buf, retPtr.get(), 0);
    smart_str_appendc(&buf, '\0');

    php_printf("%s", ZSTR_VAL(buf.s));
    smart_str_free(&buf);
}

PHP_METHOD(api, errorHandler)
{
    zend_long error_no_param;
    char *error_str_param = NULL;
    size_t error_str_param_len;
    char *error_file_param = NULL;
    size_t error_file_param_len;
    zend_long error_line_param;
    zval *error_context_param;

    ZEND_PARSE_PARAMETERS_START(2, 5)
        Z_PARAM_LONG(error_no_param);
        Z_PARAM_STRING(error_str_param, error_str_param_len);
        Z_PARAM_OPTIONAL
        Z_PARAM_STRING(error_file_param, error_str_param_len);
        Z_PARAM_LONG(error_line_param);
        Z_PARAM_ARRAY(error_context_param);
    ZEND_PARSE_PARAMETERS_END();

    zval ret;
    //zval context;
    array_init(&ret);

    //ZVAL_DUP(&context, error_context_param);

    add_assoc_long(&ret, "error_no", error_no_param);
    add_assoc_string(&ret, "err_str", error_str_param);
    add_assoc_string(&ret, "err_file", error_file_param);
    add_assoc_long(&ret, "err_line", error_line_param);
    //add_assoc_zval(&ret, "err_context", &context);
    //add_assoc_zval(&ret, "err_context", error_context_param);

    smart_str buf = {0};
    php_json_encode(&buf, &ret, 0);
    smart_str_appendc(&buf, '\0');

    php_printf("%s", ZSTR_VAL(buf.s));
    smart_str_free(&buf);

    zval_ptr_dtor(&ret);
    //zval_ptr_dtor(&context);
}

/*
PHP_METHOD(api, setFilter)
{
    char *module;
    long module_len;
    zval *filterObj;

    ZEND_PARSE_PARAMETERS_START(2, 2)
        Z_PARAM_STRING(module, module_len)
        Z_PARAM_OBJECT(filterObj)
    ZEND_PARSE_PARAMETERS_END();
}*/

PHP_METHOD(api, getApiPath)
{
    zval *ret = zend_read_static_property(api_ce, "api_path", strlen("api_path"), 0);

    RETURN_ZVAL(ret, 1, 0);
}

PHP_METHOD(api, getEnv)
{
    zval *ret = zend_read_static_property(api_ce, "env", strlen("env"), 0);

    RETURN_ZVAL(ret, 1, 0);
}

PHP_METHOD(api, getPhpSapi)
{
    zval *ret = zend_read_static_property(api_ce, "php_sapi", strlen("php_sapi"), 0);

    RETURN_ZVAL(ret, 1, 0);
}

PHP_METHOD(api, getApi)
{
    std::string api_str = Router::getInstance().getApi();

    RETURN_STRING(api_str.c_str());
}

PHP_METHOD(api, getModule)
{
    std::string module_str = Router::getInstance().getModule();

    RETURN_STRING(module_str.c_str());
}

PHP_METHOD(api, getAction)
{
    std::string action_str = Router::getInstance().getAction();

    RETURN_STRING(action_str.c_str());
}

zend_function_entry api_methods[] = {
	PHP_ME(api, __construct,         NULL,     ZEND_ACC_PUBLIC | ZEND_ACC_CTOR)
	PHP_ME(api, import, NULL, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
	PHP_ME(api, autoLoad, NULL, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
	PHP_ME(api, run, NULL, ZEND_ACC_PUBLIC)
	PHP_ME(api, exceptionHandler, NULL, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
	PHP_ME(api, errorHandler, NULL, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)

	PHP_ME(api, getApiPath, NULL, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
	PHP_ME(api, getEnv, NULL, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
	PHP_ME(api, getPhpSapi, NULL, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
	PHP_ME(api, getApi, NULL, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
	PHP_ME(api, getModule, NULL, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
	PHP_ME(api, getAction, NULL, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)

	{NULL, NULL, NULL}
};

MYAPI_STARTUP_FUNCTION(api)
{
    zend_class_entry ce;
    INIT_CLASS_ENTRY(ce, "myapi\\Api", api_methods);
    api_ce = zend_register_internal_class(&ce);
    zend_declare_property_string(api_ce, "api_path", strlen("api_path"), "", ZEND_ACC_PRIVATE | ZEND_ACC_STATIC);
    zend_declare_property_bool(api_ce, "is_instantiated", strlen("is_instantiated"), 0, ZEND_ACC_PRIVATE | ZEND_ACC_STATIC);
    zend_declare_property_null(api_ce, "env", strlen("env"), ZEND_ACC_PRIVATE | ZEND_ACC_STATIC);
    zend_declare_property_null(api_ce, "php_sapi", strlen("php_sapi"), ZEND_ACC_PRIVATE | ZEND_ACC_STATIC);

    zend_declare_property_null(api_ce, "test_var", strlen("test_var"), ZEND_ACC_PRIVATE | ZEND_ACC_STATIC);
    return SUCCESS;
}
