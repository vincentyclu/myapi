extern "C" {
    #ifdef HAVE_CONFIG_H
    # include "config.h"
    #endif

    #include "php.h"
    #include "ext/standard/info.h"
    #include "ext/standard/php_var.h"
    #include "ext/json/php_json.h"
    #include "php_myapi.h"
}

#include "api.h"
#include "MyApiTool.h"
#include <vector>
#include <string>
#include <fstream>

zend_class_entry *api_ce;

PHP_METHOD(api, __construct)
{
    char *path = "";
    size_t var_len = 0;
    std::vector<zval> v;
    zval autoload_function;

    ZEND_PARSE_PARAMETERS_START(1, 1)
        Z_PARAM_STRING(path, var_len)
    ZEND_PARSE_PARAMETERS_END();

    zend_update_static_property_string(api_ce, "api_path", strlen("api_path"), path);
    ZVAL_STRING(&autoload_function, "\\myapi\\api::autoLoad");

    v.push_back(autoload_function);

    MyApiTool::callFunction("spl_autoload_register", v);
    zval_ptr_dtor(&autoload_function);
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
        php_error_docref(NULL, E_WARNING, "FILE NOT EXISTING");
        return;
    }

    zend_op_array *arr = compile_filename(ZEND_INCLUDE_ONCE, filename);
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
        php_error_docref(NULL, E_WARNING, "FILE NOT EXISTING");
        return;
    }

    ZVAL_STRING(&compile_filename_path, include_file_path.c_str());

    zend_op_array *arr = compile_filename(ZEND_INCLUDE_ONCE, &compile_filename_path);
    zend_execute(arr, return_value);
    destroy_op_array(arr);
    efree(arr);
    zval_ptr_dtor(&compile_filename_path);

}

PHP_METHOD(api, run)
{
    zval *get_data = MyApiTool::getData();
    zend_string *module_str = zend_string_init("module", strlen("module"), 0);
    zend_string *action_str = zend_string_init("action", strlen("action"), 0);
    zval *module_name = zend_hash_find(Z_ARRVAL_P(get_data), module_str);
    zval *action_name = zend_hash_find(Z_ARRVAL_P(get_data), action_str);

    zend_string_release(module_str);
    zend_string_release(action_str);

    std::vector<zval> v;
    std::string module = "\\api\\controller\\" + std::string(Z_STRVAL_P(module_name));
    std::string action = Z_STRVAL_P(action_name);

    zval_ptr_dtor(module_name);
    zval_ptr_dtor(action_name);

    MyApiTool myApiTool(module);
    myApiTool.callMethod("__construct", v);
    zval ret = myApiTool.callMethod(action.c_str(), v);

    if (Z_TYPE(ret) == IS_UNDEF)
    {
        zval_ptr_dtor(&ret);
        return;
    }

    //smart_str buf = {0};
    //php_json_encode(&buf, &ret, 0);
    //zval_ptr_dtor(&ret);

    RETURN_ZVAL(&ret, 1, 0);
    //RETURN_STR(buf.s);

}

zend_function_entry api_methods[] = {
	PHP_ME(api, __construct,         NULL,     ZEND_ACC_PUBLIC | ZEND_ACC_CTOR)
	PHP_ME(api, import, NULL, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
	PHP_ME(api, autoLoad, NULL, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
	PHP_ME(api, run, NULL, ZEND_ACC_PUBLIC)

	{NULL, NULL, NULL}
};

MYAPI_STARTUP_FUNCTION(api)
{
    zend_class_entry ce;
    INIT_CLASS_ENTRY(ce, "myapi\\api", api_methods);
    api_ce = zend_register_internal_class(&ce);
    zend_declare_property_string(api_ce, "api_path", strlen("api_path"), "", ZEND_ACC_PRIVATE | ZEND_ACC_STATIC);

    return SUCCESS;
}
