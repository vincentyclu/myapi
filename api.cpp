extern "C" {
    #ifdef HAVE_CONFIG_H
    # include "config.h"
    #endif

    #include "php.h"
    #include "ext/standard/info.h"
    #include "ext/standard/php_var.h"
    #include "ext/json/php_json.h"
    #include "zend_smart_str.h"
    #include "ext/standard/file.h"
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

zend_class_entry *api_ce;

ZEND_BEGIN_ARG_INFO(api_construct_arg_info, 0)
    ZEND_ARG_INFO(0, api_path)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(api_import_arg_info, 0)
    ZEND_ARG_TYPE_INFO(0, filename, IS_STRING, 0)
ZEND_END_ARG_INFO()


PHP_METHOD(api, __construct)
{
    char *path = NULL;
    size_t path_len = 0;

    ZEND_PARSE_PARAMETERS_START(1, 1)
        Z_PARAM_STRING(path, path_len)
    ZEND_PARSE_PARAMETERS_END();

    zval ret;
    php_stat(path, path_len, FS_IS_DIR, &ret);

    if (Z_TYPE(ret) == IS_FALSE)
    {
        MyApiTool::throwException(MYAPI_ERR(100));
        return;
    }

    Api::getInstance().init(path);
}

PHP_METHOD(api, import)
{
    char *filename = NULL;
    size_t var_len = 0;

    ZEND_PARSE_PARAMETERS_START(1, 1)
        Z_PARAM_STRING(filename, var_len)
    ZEND_PARSE_PARAMETERS_END();

    std::shared_ptr<zval> ret = Api::getInstance().import(filename, true);

    if (!ret)
    {
        RETURN_NULL();
    }

    RETURN_ZVAL(ret.get(), 1, 0);
}

PHP_METHOD(api, autoLoad)
{

    zval* filename;
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

    std::shared_ptr<zval> ret = Api::getInstance().import(include_file_path, false);

    if (!ret)
    {
        RETURN_NULL();
    }

    RETURN_ZVAL(ret.get(), 1, 0);
}

PHP_METHOD(api, run)
{
    ZEND_PARSE_PARAMETERS_NONE();

    Api::getInstance().run();

    std::string output = Response::getInstance().output();

    RETURN_STRING(output.c_str());
}

PHP_METHOD(api, exceptionHandler)
{
    zval *obj;
    ZEND_PARSE_PARAMETERS_START(1, 1)
        Z_PARAM_OBJECT(obj)
    ZEND_PARSE_PARAMETERS_END();

    std::shared_ptr<zval> msgPtr = MyApiTool::callMethodWithObject(*obj, "getMessage");
    std::shared_ptr<zval> codePtr = MyApiTool::callMethodWithObject(*obj, "getCode");

    zval ret;
    std::shared_ptr<zval> retPtr = MyApiTool::getZval(&ret, [&ret, &msgPtr, &codePtr](){
        array_init(&ret);
        add_assoc_zval(&ret, "message", msgPtr.get());
        add_assoc_zval(&ret, "code", codePtr.get());
    });

    Response::getInstance().setErrorResult(&ret, 1);
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

    Response::getInstance().setErrorResult(&ret, 1);

    zval_ptr_dtor(&ret);
    //zval_ptr_dtor(&context);
}

PHP_METHOD(api, getApiPath)
{
    ZEND_PARSE_PARAMETERS_NONE();

    zval *ret = zend_read_static_property(api_ce, "api_path", strlen("api_path"), 0);

    RETURN_ZVAL(ret, 1, 0);
}

PHP_METHOD(api, getEnv)
{
    ZEND_PARSE_PARAMETERS_NONE();

    zval *ret = zend_read_static_property(api_ce, "env", strlen("env"), 0);

    RETURN_ZVAL(ret, 1, 0);
}

PHP_METHOD(api, getPhpSapi)
{
    ZEND_PARSE_PARAMETERS_NONE();

    zval *ret = zend_read_static_property(api_ce, "php_sapi", strlen("php_sapi"), 0);

    RETURN_ZVAL(ret, 1, 0);
}

PHP_METHOD(api, getApi)
{
    ZEND_PARSE_PARAMETERS_NONE();

    std::string api_str = Router::getInstance().getApi();

    RETURN_STRING(api_str.c_str());
}

PHP_METHOD(api, getModule)
{
    ZEND_PARSE_PARAMETERS_NONE();

    std::string module_str = Router::getInstance().getModule();

    RETURN_STRING(module_str.c_str());
}

PHP_METHOD(api, getAction)
{
    ZEND_PARSE_PARAMETERS_NONE();

    std::string action_str = Router::getInstance().getAction();

    RETURN_STRING(action_str.c_str());
}

zend_function_entry api_methods[] = {
	PHP_ME(api, __construct,      api_construct_arg_info, ZEND_ACC_PUBLIC | ZEND_ACC_CTOR)
	PHP_ME(api, import,           api_import_arg_info,    ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
	PHP_ME(api, autoLoad,         NULL,                   ZEND_ACC_PRIVATE | ZEND_ACC_STATIC)
	PHP_ME(api, run,              NULL,                   ZEND_ACC_PUBLIC)
	PHP_ME(api, exceptionHandler, NULL,                   ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
	PHP_ME(api, errorHandler,     NULL,                   ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)

	PHP_ME(api, getApiPath,       NULL,                   ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
	PHP_ME(api, getEnv,           NULL,                   ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
	PHP_ME(api, getPhpSapi,       NULL,                   ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
	PHP_ME(api, getApi,           NULL,                   ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
	PHP_ME(api, getModule,        NULL,                   ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
	PHP_ME(api, getAction,        NULL,                   ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)

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

    return SUCCESS;
}
