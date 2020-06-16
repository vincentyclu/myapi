extern "C" {
    #ifdef HAVE_CONFIG_H
    # include "config.h"
    #endif

    #include "php.h"
    #include "ext/standard/info.h"
    #include "ext/standard/php_var.h"
    #include "ext/standard/php_string.h"
}
#include "php_myapi.h"
#include "model.h"
#include "MyApiTool.h"
#include "ModelEx.h"
#include <vector>
#include <string>
#include "SqlParser.h"

zend_class_entry* model_ce;

PHP_METHOD(model, __construct)
{
    Model::getInstance().init();
}

PHP_METHOD(model, query)
{
    char *sql;
    size_t sql_len;

    ZEND_PARSE_PARAMETERS_START(1, 1)
        Z_PARAM_STRING(sql, sql_len)
    ZEND_PARSE_PARAMETERS_END();

    std::shared_ptr<zval> queryRet = Model::getInstance().query(sql);

    RETURN_ZVAL(queryRet.get(), 1, 0);
}

PHP_METHOD(model, exec)
{
    char *sql;
    size_t sql_len;

    ZEND_PARSE_PARAMETERS_START(1, 1)
        Z_PARAM_STRING(sql, sql_len)
    ZEND_PARSE_PARAMETERS_END();

    int ret = Model::getInstance().exec(sql);

    RETURN_LONG(ret);
}


PHP_METHOD(model, get)
{
    char *table_name;
    size_t table_name_len;
    zval *where = NULL;
    zval *field = NULL;
    zval *order_by = NULL;
    zval *group_by = NULL;
    zval *having = NULL;

    ZEND_PARSE_PARAMETERS_START(1, 6)
        Z_PARAM_STRING(table_name, table_name_len)
        Z_PARAM_OPTIONAL
        Z_PARAM_ZVAL(where)
        Z_PARAM_ZVAL(field)
        Z_PARAM_ZVAL(order_by)
        Z_PARAM_ZVAL(group_by)
        Z_PARAM_ZVAL(having)
    ZEND_PARSE_PARAMETERS_END();

    std::shared_ptr<zval> ret_ptr = Model::getInstance().get(table_name, where,
        field, order_by, group_by, having);

    if (!ret_ptr)
    {
        RETURN_NULL();
    }

    RETURN_ZVAL(ret_ptr.get(), 1, 0);
}


PHP_METHOD(model, add)
{
    char *table_name;
    size_t table_name_len;

    zval *data;

    ZEND_PARSE_PARAMETERS_START(2, 2)
        Z_PARAM_STRING(table_name, table_name_len)
        Z_PARAM_ARRAY(data)
    ZEND_PARSE_PARAMETERS_END();

    zend_string* key;
    zval *value;

    zval placeholder;
    zval placeholder_data;
    array_init(&placeholder);
    array_init(&placeholder_data);

    ZEND_HASH_FOREACH_STR_KEY_VAL(Z_ARRVAL_P(data), key, value){
        if (key == NULL || (Z_TYPE_P(value) < IS_LONG || Z_TYPE_P(value) > IS_STRING))
        {
            zval_ptr_dtor(&placeholder);
            zval_ptr_dtor(&placeholder_data);
            RETURN_NULL();
        }

        std::string ph_str = std::string(":") + ZSTR_VAL(key);
        add_assoc_string(&placeholder, ZSTR_VAL(key), ph_str.c_str());

        switch (Z_TYPE_P(value))
        {
            case IS_LONG:
                add_assoc_long(&placeholder_data, ph_str.c_str(), Z_LVAL_P(value));
                break;
            case IS_DOUBLE:
                add_assoc_double(&placeholder_data, ph_str.c_str(), Z_DVAL_P(value));
                break;
            case IS_STRING:
                add_assoc_string(&placeholder_data, ph_str.c_str(), Z_STRVAL_P(value));
                break;
        }

    }ZEND_HASH_FOREACH_END();

    bool ret = Model::getInstance().add(table_name, &placeholder, &placeholder_data);
    zval_ptr_dtor(&placeholder);
    zval_ptr_dtor(&placeholder_data);
    RETURN_BOOL(ret);
}


PHP_METHOD(model, update)
{
    char *table_name;
    size_t table_name_len;
    zval *where;
    zval *data;

    ZEND_PARSE_PARAMETERS_START(3, 3)
        Z_PARAM_STRING(table_name, table_name_len)
        Z_PARAM_ARRAY(where)
        Z_PARAM_ARRAY(data)
    ZEND_PARSE_PARAMETERS_END();

    zend_string* key;
    zval *value;

    zval placeholder_key_values;
    zval placeholder_where;
    zval placeholder_data;
    array_init(&placeholder_key_values);
    array_init(&placeholder_where);
    array_init(&placeholder_data);

    ZEND_HASH_FOREACH_STR_KEY_VAL(Z_ARRVAL_P(data), key, value){
        if (key == NULL || (Z_TYPE_P(value) < IS_LONG || Z_TYPE_P(value) > IS_STRING))
        {
            zval_ptr_dtor(&placeholder_key_values);
            zval_ptr_dtor(&placeholder_where);
            zval_ptr_dtor(&placeholder_data);
            RETURN_NULL();
        }

        std::string ph_str = std::string(":") + ZSTR_VAL(key);
        add_assoc_string(&placeholder_key_values, ZSTR_VAL(key), ph_str.c_str());

        switch (Z_TYPE_P(value))
        {
            case IS_LONG:
                add_assoc_long(&placeholder_data, ph_str.c_str(), Z_LVAL_P(value));
                break;
            case IS_DOUBLE:
                add_assoc_double(&placeholder_data, ph_str.c_str(), Z_DVAL_P(value));
                break;
            case IS_STRING:
                add_assoc_string(&placeholder_data, ph_str.c_str(), Z_STRVAL_P(value));
                break;
        }

    }ZEND_HASH_FOREACH_END();

    ZEND_HASH_FOREACH_STR_KEY_VAL(Z_ARRVAL_P(where), key, value){
        if (key == NULL || (Z_TYPE_P(value) < IS_LONG || Z_TYPE_P(value) > IS_ARRAY))
        {
            zval_ptr_dtor(&placeholder_key_values);
            zval_ptr_dtor(&placeholder_where);
            zval_ptr_dtor(&placeholder_data);
            RETURN_NULL();
        }

        std::string ph_str;
        if (Z_TYPE_P(value) != IS_ARRAY)
        {
            ph_str = std::string(":") + ZSTR_VAL(key);
            add_assoc_string(&placeholder_where, ZSTR_VAL(key), ph_str.c_str());
        }

        switch (Z_TYPE_P(value))
        {
            case IS_LONG:
                add_assoc_long(&placeholder_data, ph_str.c_str(), Z_LVAL_P(value));
                break;
            case IS_DOUBLE:
                add_assoc_double(&placeholder_data, ph_str.c_str(), Z_DVAL_P(value));
                break;
            case IS_STRING:
                add_assoc_string(&placeholder_data, ph_str.c_str(), Z_STRVAL_P(value));
                break;
            case IS_ARRAY:
                zval *v;
                zval inArray;
                array_init(&inArray);

                ZEND_HASH_FOREACH_VAL(Z_ARRVAL_P(value), v)
                    if (Z_TYPE_P(v) < IS_LONG || Z_TYPE_P(v) > IS_STRING)
                    {
                        zval_ptr_dtor(&placeholder_key_values);
                        zval_ptr_dtor(&placeholder_where);
                        zval_ptr_dtor(&placeholder_data);
                        zval_ptr_dtor(&inArray);
                        RETURN_NULL();
                    }

                    if (Z_TYPE_P(v) == IS_STRING)
                    {
                        std::string tmp_str = std::string(":") + Z_STRVAL_P(v);
                        add_next_index_string(&inArray, tmp_str.c_str());
                        add_assoc_string(&placeholder_data, tmp_str.c_str(), Z_STRVAL_P(v));
                    }
                    else
                    {
                        add_next_index_zval(&inArray, v);
                    }
                ZEND_HASH_FOREACH_END();

                add_assoc_zval(&placeholder_where, ZSTR_VAL(key), &inArray);
                zval_ptr_dtor(&inArray);
                break;
        }

    }ZEND_HASH_FOREACH_END();

    bool ret = Model::getInstance().update(table_name, &placeholder_key_values, &placeholder_where, &placeholder_data);
    zval_ptr_dtor(&placeholder_key_values);
    zval_ptr_dtor(&placeholder_where);
    zval_ptr_dtor(&placeholder_data);
    RETURN_BOOL(ret);
}


PHP_METHOD(model, delete)
{
    char *table_name;
    size_t table_name_len;
    zval *where;

    ZEND_PARSE_PARAMETERS_START(2, 2)
        Z_PARAM_STRING(table_name, table_name_len)
        Z_PARAM_ARRAY(where)
    ZEND_PARSE_PARAMETERS_END();

    bool ret = Model::getInstance().del(table_name, where);

    RETURN_BOOL(ret);
}

PHP_METHOD(model, addAll)
{
/*
    char *table_name;
    size_t table_name_len;

    char *key_str;
    size_t key_str_len;
    zval *data;

    ZEND_PARSE_PARAMETERS_START(3, 3)
        Z_PARAM_STRING(table_name, table_name_len)
        Z_PARAM_STRING(key_str, key_str_len)
        Z_PARAM_ARRAY(data)
    ZEND_PARSE_PARAMETERS_END();

    std::string value_str;

    zval *val;
    zend_long num;
    ZEND_HASH_FOREACH_NUM_KEY_VAL(Z_ARRVAL_P(data), num, val)
        zval ret;
        zend_string *str = zend_string_init("','", strlen("','"), 0);
        php_implode(str, val, &ret);

        if (num == 0)
        {
            value_str += "('";
        }
        else
        {
            value_str += ",('";
        }

        value_str += std::string(Z_STRVAL(ret)) + "')";

        zend_string_release(str);
        zval_ptr_dtor(&ret);
    ZEND_HASH_FOREACH_END();

    bool ret = Model::getInstance().add(table_name, key_str, value_str);

    RETURN_BOOL(ret);
*/
}

PHP_METHOD(model, pdo)
{
    zval *pdo = zend_read_static_property(model_ce, "pdo", strlen("pdo"), 0);

    RETURN_ZVAL(pdo, 1, 0);
}


zend_function_entry model_methods[] = {
	PHP_ME(model, __construct,         NULL,     ZEND_ACC_PUBLIC | ZEND_ACC_CTOR)
	PHP_ME(model, query,        NULL,     ZEND_ACC_PUBLIC)
	PHP_ME(model, exec,        NULL,     ZEND_ACC_PUBLIC)
	PHP_ME(model, get, NULL, ZEND_ACC_PUBLIC)
	PHP_ME(model, add, NULL, ZEND_ACC_PUBLIC)
	PHP_ME(model, update, NULL, ZEND_ACC_PUBLIC)
	PHP_ME(model, delete, NULL, ZEND_ACC_PUBLIC)
	PHP_ME(model, addAll, NULL, ZEND_ACC_PUBLIC)
	PHP_ME(model, pdo, NULL, ZEND_ACC_PUBLIC)

	{NULL, NULL, NULL}
};

MYAPI_STARTUP_FUNCTION(model)
{
    zend_class_entry ce;
    INIT_CLASS_ENTRY(ce, "myapi\\Model", model_methods);
    model_ce = zend_register_internal_class(&ce);

    zend_declare_property_null(model_ce, "pdo", strlen("pdo"), ZEND_ACC_PUBLIC | ZEND_ACC_STATIC);

    return SUCCESS;
}
