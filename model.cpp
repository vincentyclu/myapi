extern "C" {
    #ifdef HAVE_CONFIG_H
    # include "config.h"
    #endif

    #include "php.h"
    #include "ext/standard/info.h"
    #include "ext/standard/php_var.h"
    #include "php_myapi.h"
}

#include "model.h"
#include "MyApiTool.h"
#include <vector>
#include <string>

zend_class_entry* model_ce;

int call_function_by_ptr(zend_function *fbc, zend_object *obj, zval *retval, uint32_t num_params, zval *params) {
    zend_fcall_info fci;
    zend_fcall_info_cache fcc;

    fci.size = sizeof(fci);
    fci.object = obj;
    fci.retval = retval;
    fci.param_count = num_params;
    fci.params = params;
    fci.no_separation = 1;          // Don't allow creating references into params
    ZVAL_UNDEF(&fci.function_name); // Unused if fcc is provided

    //fcc.initialized = 1;
    fcc.function_handler = fbc;
    fcc.calling_scope = NULL;       // Appears to be dead
    fcc.called_scope = obj ? obj->ce : fbc->common.scope;
    fcc.object = obj;

    return zend_call_function(&fci, &fcc);
}


PHP_METHOD(model, __construct)
{
    zval obj;
    zval ret;
    zval function_name;
    zend_string *class_name_zstr = zend_string_init("\\PDO", strlen("\\PDO"), 0);
    object_init_ex(&obj, zend_lookup_class(class_name_zstr));
    zend_string_release(class_name_zstr);

    std::vector<zval> consParam;

    zval val1 = MyApiTool::getString("mysql:host=127.0.0.1;dbname=test;");
    zval val2 = MyApiTool::getString("root");
    zval val3 = MyApiTool::getString("");
    consParam.push_back(val1);
    consParam.push_back(val2);
    consParam.push_back(val3);

    MyApiTool::callMethodWithObject(obj, "__construct", consParam);

    zend_update_property(model_ce, getThis(), "pdo", strlen("pdo"), &obj);
    zval_ptr_dtor(&obj);

    zval_ptr_dtor(&val1);
    zval_ptr_dtor(&val2);
    zval_ptr_dtor(&val3);

}

//TODO delete?
PHP_METHOD(model, init)
{
    zval obj;
    zval ret;
    zval function_name;
    zend_string *class_name_zstr = zend_string_init("\\PDO", strlen("\\PDO"), 0);
    object_init_ex(&obj, zend_lookup_class(class_name_zstr));
    zend_string_release(class_name_zstr);

    std::vector<zval> consParam;

    zval val1 = MyApiTool::getString("mysql:host=127.0.0.1;dbname=test;");
    zval val2 = MyApiTool::getString("root");
    zval val3 = MyApiTool::getString("");
    consParam.push_back(val1);
    consParam.push_back(val2);
    consParam.push_back(val3);

    MyApiTool::callMethodWithObject(obj, "__construct", consParam);

    zend_update_property(model_ce, getThis(), "pdo", strlen("pdo"), &obj);
    zval_ptr_dtor(&obj);

    zval_ptr_dtor(&val1);
    zval_ptr_dtor(&val2);
    zval_ptr_dtor(&val3);
}

PHP_METHOD(model, query)
{
    zval *param;

    ZEND_PARSE_PARAMETERS_START(1, 1)
        Z_PARAM_ZVAL(param)
    ZEND_PARSE_PARAMETERS_END();
    zval *ret = zend_read_property(model_ce, getThis(), "pdo", strlen("pdo"), 0, NULL);

    std::vector<zval> v;
    v.push_back(*param);

    zval result = MyApiTool::callMethodWithObject(*ret, "query", v);
    //zval_ptr_dtor(&result);

    RETURN_ZVAL(&result, 1, 1);
}

PHP_METHOD(model, get)
{
    zval *table_name;
    zval *where;
    zend_long num_key;
    zend_string* string_key;
    zval *value;
    std::string where_str;

    ZEND_PARSE_PARAMETERS_START(2, 2)
        Z_PARAM_ZVAL(table_name)
        Z_PARAM_ARRAY(where)
    ZEND_PARSE_PARAMETERS_END();

    ZEND_HASH_FOREACH_KEY_VAL(Z_ARRVAL_P(where), num_key, string_key, value){
        convert_to_string(value);
        std::string str;
        str = std::string(ZSTR_VAL(string_key)) + "='" + std::string(Z_STRVAL_P(value)) + "'";

        if (where_str == "")
        {
            where_str = str;
        }
        else
        {
            where_str += " and " + str;
        }
    }ZEND_HASH_FOREACH_END();

    where_str = "SELECT * FROM " + std::string(Z_STRVAL_P(table_name)) + " WHERE " + where_str;

    zval *ret = zend_read_property(model_ce, getThis(), "pdo", strlen("pdo"), 0, NULL);

    std::vector<zval> v;
    zval param;
    ZVAL_STRING(&param, where_str.c_str());
    v.push_back(param);

    zval result = MyApiTool::callMethodWithObject(*ret, "query", v);

    ZVAL_PTR_DTOR(&param);

    RETURN_ZVAL(&result, 1, 1);
}

PHP_METHOD(model, add)
{
    zval *table_name;
    zval *data;
    zend_long num_key;
    zend_string* string_key;
    zval *value;
    std::string sql_str;
    std::string key_str;
    std::string val_str;

    ZEND_PARSE_PARAMETERS_START(2, 2)
        Z_PARAM_ZVAL(table_name)
        Z_PARAM_ARRAY(data)
    ZEND_PARSE_PARAMETERS_END();

    ZEND_HASH_FOREACH_KEY_VAL(Z_ARRVAL_P(data), num_key, string_key, value){
        convert_to_string(value);

        if (key_str == "")
        {
            key_str = std::string(ZSTR_VAL(string_key));
        }
        else
        {
            key_str += "," + std::string(ZSTR_VAL(string_key));
        }

        if (val_str == "")
        {
            val_str = "'" + std::string(Z_STRVAL_P(value)) + "'";
        }
        else
        {
            val_str += ",'" + std::string(Z_STRVAL_P(value)) + "'";
        }
    }ZEND_HASH_FOREACH_END();

    sql_str = "INSERT INTO " + std::string(Z_STRVAL_P(table_name)) + "(" + key_str + ") VALUES (" + val_str + ")";

    zval *ret = zend_read_property(model_ce, getThis(), "pdo", strlen("pdo"), 0, NULL);

    std::vector<zval> v;
    zval param;
    ZVAL_STRING(&param, sql_str.c_str());
    v.push_back(param);

    zval result = MyApiTool::callMethodWithObject(*ret, "query", v);

    ZVAL_PTR_DTOR(&param);

    RETURN_ZVAL(&result, 1, 1);
}

PHP_METHOD(model, update)
{
    zval *table_name;
    zval *where;
    zend_long num_key;
    zend_string* string_key;
    zval *value;
    std::string where_str;

    zval *data;
    zend_long data_num_key;
    zend_string* data_string_key;
    zval *data_value;
    std::string update_str;

    std::string sql_str;

    ZEND_PARSE_PARAMETERS_START(3, 3)
        Z_PARAM_ZVAL(table_name)
        Z_PARAM_ARRAY(where)
        Z_PARAM_ARRAY(data)
    ZEND_PARSE_PARAMETERS_END();

    ZEND_HASH_FOREACH_KEY_VAL(Z_ARRVAL_P(where), num_key, string_key, value){
        std::string str;
        convert_to_string(value);
        str = std::string(ZSTR_VAL(string_key)) + "='" + std::string(Z_STRVAL_P(value)) + "'";

        if (where_str == "")
        {
            where_str = str;
        }
        else
        {
            where_str += " and " + str;
        }
    }ZEND_HASH_FOREACH_END();

    ZEND_HASH_FOREACH_KEY_VAL(Z_ARRVAL_P(data), data_num_key, data_string_key, data_value){
        std::string str;
        convert_to_string(data_value);
        str = std::string(ZSTR_VAL(data_string_key)) + "='" + std::string(Z_STRVAL_P(data_value)) + "'";

        if (update_str == "")
        {
            update_str = str;
        }
        else
        {
            update_str += "," + str;
        }
    }ZEND_HASH_FOREACH_END();

    sql_str = "UPDATE " + std::string(Z_STRVAL_P(table_name)) + " SET " + update_str + " WHERE " + where_str;

    zval *ret = zend_read_property(model_ce, getThis(), "pdo", strlen("pdo"), 0, NULL);

    std::vector<zval> v;
    zval param = MyApiTool::getString(sql_str);
    v.push_back(param);

    zval result = MyApiTool::callMethodWithObject(*ret, "query", v);

    ZVAL_PTR_DTOR(&param);

    RETURN_ZVAL(&result, 1, 1);

}

PHP_METHOD(model, delete)
{
    zval *table_name;
    zval *where;
    zend_long num_key;
    zend_string* string_key;
    zval *value;
    std::string where_str;

    ZEND_PARSE_PARAMETERS_START(2, 2)
        Z_PARAM_ZVAL(table_name)
        Z_PARAM_ARRAY(where)
    ZEND_PARSE_PARAMETERS_END();

    ZEND_HASH_FOREACH_KEY_VAL(Z_ARRVAL_P(where), num_key, string_key, value){
        std::string str;
        convert_to_string(value);
        str = std::string(ZSTR_VAL(string_key)) + "='" + std::string(Z_STRVAL_P(value)) + "'";

        if (where_str == "")
        {
            where_str = str;
        }
        else
        {
            where_str += " and " + str;
        }
    }ZEND_HASH_FOREACH_END();

    where_str = "DELETE FROM " + std::string(Z_STRVAL_P(table_name)) + " WHERE " + where_str;

    zval *ret = zend_read_property(model_ce, getThis(), "pdo", strlen("pdo"), 0, NULL);

    std::vector<zval> v;
    zval param;
    ZVAL_STRING(&param, where_str.c_str());
    v.push_back(param);

    zval result = MyApiTool::callMethodWithObject(*ret, "query", v);

    ZVAL_PTR_DTOR(&param);

    RETURN_ZVAL(&result, 1, 1);
}

zend_function_entry model_methods[] = {
	PHP_ME(model, __construct,         NULL,     ZEND_ACC_PUBLIC | ZEND_ACC_CTOR)
	PHP_ME(model, init,         NULL,     ZEND_ACC_PUBLIC)
	PHP_ME(model, query,        NULL,     ZEND_ACC_PUBLIC)
	PHP_ME(model, get, NULL, ZEND_ACC_PUBLIC)
	PHP_ME(model, add, NULL, ZEND_ACC_PUBLIC)
	PHP_ME(model, update, NULL, ZEND_ACC_PUBLIC)
	PHP_ME(model, delete, NULL, ZEND_ACC_PUBLIC)

	{NULL, NULL, NULL}
};

MYAPI_STARTUP_FUNCTION(model)
{
    zend_class_entry ce;
    INIT_CLASS_ENTRY(ce, "myapi\\Model", model_methods);
    model_ce = zend_register_internal_class(&ce);

    zend_declare_property_null(model_ce, "pdo", strlen("pdo"), ZEND_ACC_PUBLIC);

    return SUCCESS;
}
