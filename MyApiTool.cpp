#include "MyApiTool.h"
#include <vector>

MyApiTool::MyApiTool()
{
}

MyApiTool::~MyApiTool()
{
    zval_ptr_dtor(&this->object);
}

MyApiTool::MyApiTool(std::string className)
{
    zend_string *class_name_zstr = zend_string_init(className.c_str(), strlen(className.c_str()), 0);
    object_init_ex(&this->object, zend_fetch_class(class_name_zstr, ZEND_FETCH_CLASS_AUTO));
    zend_string_release(class_name_zstr);
}

zval MyApiTool::callMethod(const char *function_name, std::vector<zval> vec)
{
    return MyApiTool::callMethodWithObject(this->object, function_name, vec);
}

int MyApiTool::getNum()
{
    return 555;
}

zval MyApiTool::callFunction(const char *function_name, std::vector<zval> vec)
{
    int params_count;
    zval ret;
    zval function_name_zval;
    zval *params;

    params_count = vec.size();

    if (params_count > 0)
    {
        params = new zval[params_count];

        for (auto i = 0; i < params_count; i++)
        {
            params[i] = vec[i];
        }
    }

    ZVAL_STRING(&function_name_zval, function_name);

    int call_result = call_user_function(NULL, NULL, &function_name_zval, &ret, params_count, params);
    zval_ptr_dtor(&function_name_zval);

    if (params_count > 0)
    {
        delete[] params;
    }

    if (FAILURE == call_result)
    {
        php_error_docref(NULL, E_WARNING, "FUNCTION NOT EXISTING");
        return ret;
    }

    return ret;
}

zval MyApiTool::callMethod(const char* class_name, const char *function_name, std::vector<zval> vec)
{
    int params_count;
    zval ret;
    zval function_name_zval;
    zval *params;
    zval obj;
    zend_string* class_name_zstr;

    params_count = vec.size();

    if (params_count > 0)
    {
        params = new zval[params_count];

        for (auto i = 0; i < params_count; i++)
        {
            params[i] = vec[i];
        }
    }

    class_name_zstr = zend_string_init(class_name, strlen(class_name), 0);
    object_init_ex(&obj, zend_fetch_class(class_name_zstr, ZEND_FETCH_CLASS_AUTO));

    ZVAL_STRING(&function_name_zval, function_name);

    int call_result = call_user_function(NULL, &obj, &function_name_zval, &ret, params_count, params);

    zval_ptr_dtor(&function_name_zval);
    zend_string_release(class_name_zstr);
    zval_ptr_dtor(&obj);

    if (params_count > 0)
    {
        delete[] params;
    }

    if (FAILURE == call_result)
    {
        php_error_docref(NULL, E_WARNING, "FUNCTION NOT EXISTING");
        return ret;
    }

    return ret;
}

zval MyApiTool::callMethodWithObject(zval& obj, const char *function_name, std::vector<zval> vec)
{
    int params_count;
    zval ret;
    zval function_name_zval;
    zval *params = NULL;

    params_count = vec.size();

    if (params_count > 0)
    {
        params = new zval[params_count];

        for (auto i = 0; i < params_count; i++)
        {
            params[i] = vec[i];
        }
    }

    ZVAL_STRING(&function_name_zval, function_name);

    int call_result = call_user_function(NULL, &obj, &function_name_zval, &ret, params_count, params);

    zval_ptr_dtor(&function_name_zval);

    if (params_count > 0)
    {
        delete[] params;
    }

    if (FAILURE == call_result)
    {
        php_error_docref(NULL, E_WARNING, "FUNCTION NOT EXISTING");
        return ret;
    }

    return ret;
}

zval* MyApiTool::getData()
{
    zend_string *key_get = zend_string_init("_GET", strlen("_GET"), 0);

    zval *get_data = zend_hash_find(&EG(symbol_table), key_get);
    zend_string_release(key_get);

    return get_data;
}

zval MyApiTool::getLong(long num)
{
    zval val;
    ZVAL_LONG(&val, num);

    return val;
}

zval MyApiTool::getString(std::string str)
{
    zval val;
    ZVAL_STRING(&val, str.c_str());

    return val;
}

zval MyApiTool::getString(zend_string* str)
{
    zval val;
    ZVAL_STR(&val, str);

    return val;
}
