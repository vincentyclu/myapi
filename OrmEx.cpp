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
#include "OrmEx.h"
#include "ModelEx.h"
#include "SqlParser.h"

extern zend_class_entry* orm_ce;

Orm Orm::getInstance()
{
    static Orm orm;

    return orm;
}

bool Orm::save(zval *obj)
{
    std::string tableName = Orm::getTableName(obj);
    std::string primaryKey = Orm::getPrimaryKey(obj);

    if (tableName == "" || primaryKey == "")
    {
        return false;
    }

    zval data;
    array_init(&data);

    bool initRet = this->initData(obj, &data, NULL);

    if (!initRet)
    {
        zval_ptr_dtor(&data);
        return false;
    }

    bool ret = Model::getInstance().add(tableName, &data);
    zval_ptr_dtor(&data);

    return ret;
}

bool Orm::update(zval *obj)
{
    std::string tableName = Orm::getTableName(obj);
    std::string primaryKey = Orm::getPrimaryKey(obj);

    if (tableName == "" || primaryKey == "")
    {
        return false;
    }

    zval data;
    zval where;
    array_init(&data);
    array_init(&where);

    bool initRet = this->initData(obj, &data, &where);

    if (!initRet)
    {
        zval_ptr_dtor(&data);
        zval_ptr_dtor(&where);
        return false;
    }

    bool ret = Model::getInstance().update(tableName, &data, &where);
    zval_ptr_dtor(&data);
    zval_ptr_dtor(&where);

    return ret;
}

bool Orm::del(zval *obj)
{
    std::string tableName = Orm::getTableName(obj);
    std::string primaryKey = Orm::getPrimaryKey(obj);

    if (tableName == "" || primaryKey == "")
    {
        return false;
    }

    zval where;
    array_init(&where);

    HashTable * foreachHt = Z_OBJPROP_P(obj);
    zend_class_entry *entry = Z_OBJCE_P(obj);
    int classNameLen = ZSTR_LEN(entry->name);

    zend_string *key;
    zval *propValue;

    ZEND_HASH_FOREACH_STR_KEY_VAL(foreachHt, key, propValue)
        zval *value = Z_INDIRECT_P(propValue);
        std::string key_str;

        if (strlen(ZSTR_VAL(key)) > 0)
        {
            key_str = ZSTR_VAL(key);
        }
        else
        {
            char *tmpStr = ZSTR_VAL(key);
            tmpStr += classNameLen + 2;
            key_str = tmpStr;
        }

        if (key_str == primaryKey)
        {
            switch (Z_TYPE_P(value))
            {
                case IS_LONG:
                    add_assoc_long(&where, primaryKey.c_str(), Z_LVAL_P(value));
                    break;
                case IS_DOUBLE:
                    add_assoc_double(&where, primaryKey.c_str(), Z_DVAL_P(value));
                    break;
                case IS_STRING:
                    add_assoc_string(&where, primaryKey.c_str(), Z_STRVAL_P(value));
                    break;
            }

            break;
        }
    ZEND_HASH_FOREACH_END();

    if (Z_ARRVAL_P(&where)->nNumOfElements == 0)
    {
        zval_ptr_dtor(&where);
        return false;
    }

    bool ret = Model::getInstance().del(tableName, &where);
    zval_ptr_dtor(&where);

    return ret;
}

bool Orm::initData(zval *obj, zval *data, zval *where)
{
    HashTable * foreachHt = Z_OBJPROP_P(obj);
    zend_class_entry *entry = Z_OBJCE_P(obj);
    int classNameLen = ZSTR_LEN(entry->name);
    std::string primaryKey = Orm::getPrimaryKey(obj);

    zend_string *key;
    zval *propValue;

    ZEND_HASH_FOREACH_STR_KEY_VAL(foreachHt, key, propValue)
        zval *value = Z_INDIRECT_P(propValue);

        std::string key_str;

        if (strlen(ZSTR_VAL(key)) > 0)
        {
            key_str = ZSTR_VAL(key);
        }
        else
        {
            char *tmpStr = ZSTR_VAL(key);
            tmpStr += classNameLen + 2;
            key_str = tmpStr;
        }

        if (Z_TYPE_P(value) == IS_NULL)
        {
            if (key_str == primaryKey && where)
            {
                php_error_docref(NULL, E_NOTICE, "primary key missing");
                return false;
            }

            continue;
        }

        if (key == NULL || (Z_TYPE_P(value) < IS_LONG || Z_TYPE_P(value) > IS_STRING))
        {
            return false;
        }

        if (key_str == primaryKey)
        {
            if (where)
            {
                if (Z_TYPE_P(value) == IS_STRING)
                {
                    add_assoc_string(where, key_str.c_str(), Z_STRVAL_P(value));
                }
                else if (Z_TYPE_P(value) == IS_LONG)
                {
                    add_assoc_long(where, key_str.c_str(), Z_LVAL_P(value));
                }
                else if (Z_TYPE_P(value) == IS_DOUBLE)
                {
                    add_assoc_double(where, key_str.c_str(), Z_DVAL_P(value));
                }
                else
                {
                    return false;
                }
            }
        }
        else
        {
            if (Z_TYPE_P(value) == IS_STRING)
            {
                add_assoc_string(data, key_str.c_str(), Z_STRVAL_P(value));
            }
            else if (Z_TYPE_P(value) == IS_LONG)
            {
                add_assoc_long(data, key_str.c_str(), Z_LVAL_P(value));
            }
            else if (Z_TYPE_P(value) == IS_DOUBLE)
            {
                add_assoc_double(data, key_str.c_str(), Z_DVAL_P(value));
            }
            else
            {
                continue;
            }
        }

    ZEND_HASH_FOREACH_END();

    return true;
}

std::string Orm::getTableName(zval *obj)
{
    if (!obj)
    {
        return "";
    }

    zend_class_entry *entry = Z_OBJCE_P(obj);
    std::string className = ZSTR_VAL(entry->name);
    std::string contantKey = className + "::TABLE_NAME";
    std::shared_ptr<zval> contantPtr = MyApiTool::getContant(contantKey.c_str(), entry);

    if (contantPtr && Z_TYPE_P(contantPtr.get()) == IS_STRING)
    {
        return Z_STRVAL_P(contantPtr.get());
    }

    return "";
}

std::string Orm::getPrimaryKey(zval *obj)
{
    if (!obj)
    {
        return "";
    }

    zend_class_entry *entry = Z_OBJCE_P(obj);
    std::string className = ZSTR_VAL(entry->name);
    std::string contantKey = className + "::PRIMARY_KEY";
    std::shared_ptr<zval> contantPtr = MyApiTool::getContant(contantKey.c_str(), entry);

    if (contantPtr && Z_TYPE_P(contantPtr.get()) == IS_STRING)
    {
        return Z_STRVAL_P(contantPtr.get());
    }

    return "";
}

bool Orm::updateWhereSql(zval *obj, zval *where, Orm::WhereType wt)
{
    zval *where_str = zend_read_property(orm_ce, obj, "where", strlen("where"), 0, NULL);

    SqlParser parser;

    if (Z_TYPE_P(where) != IS_STRING && Z_TYPE_P(where) != IS_ARRAY)
    {
        return false;
    }

    if (Z_TYPE_P(where_str) == IS_NULL && (wt != Orm::WhereType::WHERE && wt != Orm::WhereType::WHERE_OR))
    {
        php_error_docref(NULL, E_WARNING, "WHERE OR WHEREOR MUST BE CALLED");
        return false;
    }

    if (Z_TYPE_P(where_str) != IS_NULL && (wt == Orm::WhereType::WHERE || wt == Orm::WhereType::WHERE_OR))
    {
        php_error_docref(NULL, E_WARNING, "WHERE MUST NOT BE CALLED TWICE");
        return false;
    }

    switch (wt)
    {
        case Orm::WhereType::WHERE:
            parser.where(where);
            break;
        case Orm::WhereType::AND_WHERE:
            parser.andWhere(where);
            break;
        case Orm::WhereType::OR_WHERE:
            parser.orWhere(where);
            break;
        case Orm::WhereType::WHERE_OR:
            parser.whereOr(where);
            break;
        case Orm::WhereType::AND_WHERE_OR:
            parser.andWhereOr(where);
            break;
        case Orm::WhereType::OR_WHERE_OR:
            parser.orWhereOr(where);
            break;
    }

    std::string sql = parser.getWhereSql();

    zend_update_property_string(orm_ce, obj, "where", strlen("where"), sql.c_str());

    return true;
}

std::shared_ptr<zval> Orm::get(zval *obj)
{
    std::string tableName = Orm::getTableName(obj);
    zval *where = zend_read_property(orm_ce, obj, "where", strlen("where"), 0, NULL);
    zval *field = zend_read_property(orm_ce, obj, "field", strlen("field"), 0, NULL);
    zval *orderBy = zend_read_property(orm_ce, obj, "order_by", strlen("order_by"), 0, NULL);
    zval *groupBy = zend_read_property(orm_ce, obj, "group_by", strlen("group_by"), 0, NULL);
    zval *having = zend_read_property(orm_ce, obj, "having", strlen("having"), 0, NULL);
    zval *className = zend_read_property(orm_ce, obj, "class_name", strlen("class_name"), 0, NULL);

    if (!className || Z_TYPE_P(className) != IS_STRING)
    {
        return std::shared_ptr<zval>();
    }

    //Model::getInstance().init();
    std::shared_ptr<zval> ret = Model::getInstance().get(tableName, where, field, orderBy, groupBy,
        having, "fetchObjects", Z_STRVAL_P(className));

    return ret;
}

std::shared_ptr<zval> Orm::getOne(zval *obj)
{
    std::string tableName = Orm::getTableName(obj);
    zval *where = zend_read_property(orm_ce, obj, "where", strlen("where"), 0, NULL);
    zval *field = zend_read_property(orm_ce, obj, "field", strlen("field"), 0, NULL);
    zval *orderBy = zend_read_property(orm_ce, obj, "order_by", strlen("order_by"), 0, NULL);
    zval *groupBy = zend_read_property(orm_ce, obj, "group_by", strlen("group_by"), 0, NULL);
    zval *having = zend_read_property(orm_ce, obj, "having", strlen("having"), 0, NULL);
    zval *className = zend_read_property(orm_ce, obj, "class_name", strlen("class_name"), 0, NULL);

    if (!className || Z_TYPE_P(className) != IS_STRING)
    {
        return std::shared_ptr<zval>();
    }

    std::shared_ptr<zval> ret = Model::getInstance().get(tableName, where, field, orderBy, groupBy,
        having, "fetchObject", Z_STRVAL_P(className));

    return ret;
}
