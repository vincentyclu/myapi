
extern "C" {
    #ifdef HAVE_CONFIG_H
    # include "config.h"
    #endif

    #include "php.h"
    #include "ext/standard/info.h"
    #include "ext/standard/php_var.h"
    #include "zend_object_handlers.h"
}
#include "php_myapi.h"
#include "orm.h"
#include "MyApiTool.h"
#include "ModelEx.h"
#include <string>
#include "OrmEx.h"

zend_class_entry* orm_ce;

ZEND_BEGIN_ARG_INFO(orm_arg_info__call, 0)
    ZEND_ARG_INFO(0, name)
    ZEND_ARG_INFO(0, param)
ZEND_END_ARG_INFO();

PHP_METHOD(orm, __construct)
{
    Model::getInstance().init();
}

PHP_METHOD(orm, save)
{
    bool ret = Orm::getInstance().save(getThis());

    RETURN_BOOL(ret);
}

PHP_METHOD(orm, update)
{
    bool ret = Orm::getInstance().update(getThis());

    RETURN_BOOL(ret);
}

PHP_METHOD(orm, delete)
{
    bool ret = Orm::getInstance().del(getThis());

    RETURN_BOOL(ret);
}

PHP_METHOD(orm, get)
{
    std::shared_ptr<zval> ret = Orm::getInstance().get(getThis());

    if (!ret)
    {
        RETURN_NULL();
    }

    RETURN_ZVAL(ret.get(), 1, 0);
}

PHP_METHOD(orm, getOne)
{
    std::shared_ptr<zval> ret = Orm::getInstance().getOne(getThis());

    if (!ret)
    {
        RETURN_NULL();
    }

    RETURN_ZVAL(ret.get(), 1, 0);
}

PHP_METHOD(orm, select)
{
    zval *param;
    ZEND_PARSE_PARAMETERS_START(1, 1)
        Z_PARAM_ZVAL(param)
    ZEND_PARSE_PARAMETERS_END();

    zend_update_property(orm_ce, getThis(), "select", strlen("select"), param);
}

PHP_METHOD(orm, orderBy)
{
    zval *param;
    ZEND_PARSE_PARAMETERS_START(1, 1)
        Z_PARAM_ZVAL(param)
    ZEND_PARSE_PARAMETERS_END();

    zend_update_property(orm_ce, getThis(), "order_by", strlen("order_by"), param);
}

PHP_METHOD(orm, groupBy)
{
    zval *param;
    ZEND_PARSE_PARAMETERS_START(1, 1)
        Z_PARAM_ZVAL(param)
    ZEND_PARSE_PARAMETERS_END();

    zend_update_property(orm_ce, getThis(), "group_by", strlen("group_by"), param);
}

PHP_METHOD(orm, having)
{
    zval *param;
    ZEND_PARSE_PARAMETERS_START(1, 1)
        Z_PARAM_ZVAL(param)
    ZEND_PARSE_PARAMETERS_END();

    zend_update_property(orm_ce, getThis(), "having", strlen("having"), param);
}

PHP_METHOD(orm, where)
{
    zval *param;
    ZEND_PARSE_PARAMETERS_START(1, 1)
        Z_PARAM_ZVAL(param)
    ZEND_PARSE_PARAMETERS_END();

    Orm::getInstance().updateWhereSql(getThis(), param, Orm::WhereType::WHERE);

    RETURN_ZVAL(getThis(), 1, 0);
}

PHP_METHOD(orm, andWhere)
{
    zval *param;
    ZEND_PARSE_PARAMETERS_START(1, 1)
        Z_PARAM_ZVAL(param)
    ZEND_PARSE_PARAMETERS_END();

    Orm::getInstance().updateWhereSql(getThis(), param, Orm::WhereType::AND_WHERE);

    RETURN_ZVAL(getThis(), 1, 0);
}

PHP_METHOD(orm, orWhere)
{
    zval *param;
    ZEND_PARSE_PARAMETERS_START(1, 1)
        Z_PARAM_ZVAL(param)
    ZEND_PARSE_PARAMETERS_END();

    Orm::getInstance().updateWhereSql(getThis(), param, Orm::WhereType::OR_WHERE);

    RETURN_ZVAL(getThis(), 1, 0);
}

PHP_METHOD(orm, whereOr)
{
    zval *param;
    ZEND_PARSE_PARAMETERS_START(1, 1)
        Z_PARAM_ZVAL(param)
    ZEND_PARSE_PARAMETERS_END();

    Orm::getInstance().updateWhereSql(getThis(), param, Orm::WhereType::WHERE_OR);

    RETURN_ZVAL(getThis(), 1, 0);
}

PHP_METHOD(orm, andWhereOr)
{
    zval *param;
    ZEND_PARSE_PARAMETERS_START(1, 1)
        Z_PARAM_ZVAL(param)
    ZEND_PARSE_PARAMETERS_END();

    Orm::getInstance().updateWhereSql(getThis(), param, Orm::WhereType::AND_WHERE_OR);

    RETURN_ZVAL(getThis(), 1, 0);
}

PHP_METHOD(orm, orWhereOr)
{
    zval *param;
    ZEND_PARSE_PARAMETERS_START(1, 1)
        Z_PARAM_ZVAL(param)
    ZEND_PARSE_PARAMETERS_END();

    Orm::getInstance().updateWhereSql(getThis(), param, Orm::WhereType::OR_WHERE_OR);

    RETURN_ZVAL(getThis(), 1, 0);
}

PHP_METHOD(orm, getTable)
{
    std::shared_ptr<zval> ptr = MyApiTool::callFunction("get_called_class");

    if (!ptr)
    {
        RETURN_NULL();
    }

    MyApiTool modelObj(Z_STRVAL_P(ptr.get()));
    modelObj.callMethod("__construct");

    zval obj = modelObj.getObject();
    zend_update_property_string(orm_ce, &obj, "class_name", strlen("class_name"), Z_STRVAL_P(ptr.get()));

    RETURN_ZVAL(&obj, 1, 0);
}

PHP_METHOD(orm, __call)
{
    char* name = NULL;
    size_t name_len;

    zval *param;
    ZEND_PARSE_PARAMETERS_START(2, 2)
        Z_PARAM_STRING(name, name_len);
        Z_PARAM_ARRAY(param)
    ZEND_PARSE_PARAMETERS_END();

    std::string fun_name = name;

    if (fun_name.find("get") != std::string::npos)
    {
        fun_name = fun_name.substr(3);
        fun_name[0] = std::tolower(fun_name[0]);

        zval *val = zend_read_property(orm_ce, getThis(), fun_name.c_str(), fun_name.length(), 0, NULL);

        if (!val)
        {
            RETURN_NULL();
        }

        RETURN_ZVAL(val, 1, 0);
    }
    else if (fun_name.find("set") != std::string::npos)
    {
        fun_name = fun_name.substr(3);
        fun_name[0] = std::tolower(fun_name[0]);

        std::shared_ptr<zval> ptr = MyApiTool::getZvalByHashTable(Z_ARRVAL_P(param), (zend_long) 0, false);

        if (!ptr)
        {
            RETURN_NULL();
        }

        zend_update_property(orm_ce, getThis(), fun_name.c_str(), fun_name.length(), ptr.get());
    }
    else
    {
        php_error_docref(NULL, E_NOTICE, "%s NOT FOUND", name);
        RETURN_NULL();
    }
}

zend_function_entry orm_methods[] = {
	PHP_ME(orm, __construct,         NULL,     ZEND_ACC_PUBLIC | ZEND_ACC_CTOR)
    PHP_ME(orm, save,         NULL,     ZEND_ACC_PUBLIC)
    PHP_ME(orm, update,         NULL,     ZEND_ACC_PUBLIC)
    PHP_ME(orm, delete,         NULL,     ZEND_ACC_PUBLIC)

    PHP_ME(orm, get,         NULL,     ZEND_ACC_PUBLIC)
    PHP_ME(orm, getOne,         NULL,     ZEND_ACC_PUBLIC)

    PHP_ME(orm, select, NULL, ZEND_ACC_PUBLIC)

    PHP_ME(orm, where, NULL, ZEND_ACC_PUBLIC)
    PHP_ME(orm, andWhere, NULL, ZEND_ACC_PUBLIC)
    PHP_ME(orm, orWhere, NULL, ZEND_ACC_PUBLIC)
    PHP_ME(orm, whereOr, NULL, ZEND_ACC_PUBLIC)
    PHP_ME(orm, andWhereOr, NULL, ZEND_ACC_PUBLIC)
    PHP_ME(orm, orWhereOr, NULL, ZEND_ACC_PUBLIC)

    PHP_ME(orm, orderBy, NULL, ZEND_ACC_PUBLIC)
    PHP_ME(orm, groupBy, NULL, ZEND_ACC_PUBLIC)
    PHP_ME(orm, having, NULL, ZEND_ACC_PUBLIC)
    PHP_ME(orm, getTable, NULL, ZEND_ACC_PUBLIC| ZEND_ACC_STATIC)
    PHP_ME(orm, __call, orm_arg_info__call, ZEND_ACC_PUBLIC)
	{NULL, NULL, NULL}
};

MYAPI_STARTUP_FUNCTION(orm)
{
    zend_class_entry ce;
    INIT_CLASS_ENTRY(ce, "myapi\\Orm", orm_methods);

    orm_ce = zend_register_internal_class(&ce);

    zend_declare_property_null(orm_ce, "where", strlen("where"), ZEND_ACC_PRIVATE);
    zend_declare_property_null(orm_ce, "field", strlen("field"), ZEND_ACC_PRIVATE);
    zend_declare_property_null(orm_ce, "order_by", strlen("order_by"), ZEND_ACC_PRIVATE);
    zend_declare_property_null(orm_ce, "group_by", strlen("group_by"), ZEND_ACC_PRIVATE);
    zend_declare_property_null(orm_ce, "having", strlen("having"), ZEND_ACC_PRIVATE);
    zend_declare_property_null(orm_ce, "class_name", strlen("class_name"), ZEND_ACC_PRIVATE);

    return SUCCESS;
}

