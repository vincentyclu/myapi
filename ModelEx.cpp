extern "C" {
    #ifdef HAVE_CONFIG_H
    # include "config.h"
    #endif

    #include "php.h"
    #include "ext/standard/info.h"
    #include "ext/standard/php_var.h"
    #include "ext/json/php_json.h"
    #include "zend_smart_str.h"
    #include "ext/pdo/php_pdo_driver.h"
}
#include "php_myapi.h"
#include "MyApiTool.h"
#include "ModelEx.h"
#include "ConfigEx.h"
#include "SqlParser.h"
#include "LogEx.h"

extern zend_class_entry *model_ce;

void Model::init()
{
    zval *pdo_obj = zend_read_static_property(model_ce, "pdo", strlen("pdo"), 0);

    if (!pdo_obj || Z_TYPE_P(pdo_obj) == IS_NULL)
    {

        std::shared_ptr<zval> dbConfig = Config::getInstance().get("db", true);

        if (dbConfig)
        {
            zval* dsnPtr = MyApiTool::getZvalByHashTable(Z_ARRVAL_P(dbConfig.get()), "dsn");
            zval* userPtr = MyApiTool::getZvalByHashTable(Z_ARRVAL_P(dbConfig.get()), "user");
            zval* passwordPtr = MyApiTool::getZvalByHashTable(Z_ARRVAL_P(dbConfig.get()), "password");
            zval* optionsPtr = MyApiTool::getZvalByHashTable(Z_ARRVAL_P(dbConfig.get()), "options");

            if (!dsnPtr || !userPtr || !passwordPtr || !optionsPtr)
            {
                return;
            }

            MyApiTool myapiTool("\\PDO");
            zval params[] = {*dsnPtr, *userPtr, *passwordPtr, *optionsPtr};
            myapiTool.callMethod("__construct", 4, params);

            zval object = myapiTool.getObject();

            zend_update_static_property(model_ce, "pdo", strlen("pdo"), &object);
        }
    }
}

std::shared_ptr<zval> Model::query(std::string sql)
{
    zval *pdo_obj = zend_read_static_property(model_ce, "pdo", strlen("pdo"), 0);

    Log::getInstance().writeLog("SQL INFO:" + sql, "debug");

    zval param;
    ZVAL_STRING(&param, sql.c_str());
    zval params[] = {param};

    std::shared_ptr<zval> queryRet = MyApiTool::callMethodWithObject(*pdo_obj, "query", 1, params);
    zval_ptr_dtor(&param);

    return queryRet;
}

int Model::exec(std::string sql)
{
    zval *pdo_obj = zend_read_static_property(model_ce, "pdo", strlen("pdo"), 0);

    Log::getInstance().writeLog("SQL INFO:" + sql, "debug");

    zval param;
    ZVAL_STRING(&param, sql.c_str());
    zval params[] = {param};

    std::shared_ptr<zval> ret = MyApiTool::callMethodWithObject(*pdo_obj, "exec", 1, params);
    zval_ptr_dtor(&param);

    if (!ret)
    {
        return 0;
    }

    return Z_LVAL_P(ret.get());
}

std::shared_ptr<zval> Model::prepare(std::string sql)
{
    zval *pdo_obj = zend_read_static_property(model_ce, "pdo", strlen("pdo"), 0);

    if (!pdo_obj || Z_TYPE_P(pdo_obj) != IS_OBJECT)
    {
        php_error_docref(NULL, E_WARNING, "pdo should be initialized");
        return std::shared_ptr<zval>();
    }

    Log::getInstance().writeLog("SQL INFO:" + sql, "debug");

    zval param;
    ZVAL_STRING(&param, sql.c_str());
    zval params[] = {param};
    std::shared_ptr<zval> preparePtr = MyApiTool::callMethodWithObject(*pdo_obj, "prepare", 1, params);
    zval_ptr_dtor(&param);

    return preparePtr;
}

bool Model::execute(zval*stmt, zval *param)
{
    std::shared_ptr<zval> execPtr;

    if (param == NULL)
    {
        execPtr = MyApiTool::callMethodWithObject(*stmt, "execute", 0, NULL);
    }
    else
    {
        zval execParams[] = {*param};
        execPtr = MyApiTool::callMethodWithObject(*stmt, "execute", 1, execParams);
    }

    if (execPtr && Z_TYPE_P(execPtr.get()) == IS_TRUE)
    {
        return true;
    }

    return false;
}

std::shared_ptr<zval> Model::fetchAll(zval*stmt)
{
    zval param;
    ZVAL_LONG(&param, PDO_FETCH_ASSOC);
    zval params[] = {param};
    std::shared_ptr<zval> ret = MyApiTool::callMethodWithObject(*stmt, "fetchAll", 1, params);

    return ret;
}

std::shared_ptr<zval> Model::fetchOne(zval*stmt)
{
    zval param;
    ZVAL_LONG(&param, PDO_FETCH_ASSOC);
    zval params[] = {param};
    std::shared_ptr<zval> ret = MyApiTool::callMethodWithObject(*stmt, "fetch", 1, params);

    return ret;
}

std::shared_ptr<zval> Model::fetchObjects(zval*stmt, std::string className)
{
    zval param;
    ZVAL_LONG(&param, PDO_FETCH_CLASS);

    zval classNameParam;
    ZVAL_STRING(&classNameParam, className.c_str());
    zval params[] = {param, classNameParam};
    std::shared_ptr<zval> ret = MyApiTool::callMethodWithObject(*stmt, "fetchAll", 2, params);
    zval_ptr_dtor(&classNameParam);

    return ret;
}

std::shared_ptr<zval> Model::fetchObject(zval*stmt, std::string className)
{
    zval classNameParam;
    ZVAL_STRING(&classNameParam, className.c_str());
    zval params[] = {classNameParam};
    std::shared_ptr<zval> ret = MyApiTool::callMethodWithObject(*stmt, "fetchObject", 1, params);
    zval_ptr_dtor(&classNameParam);

    return ret;
}

int Model::rowCount(zval*stmt)
{
    std::shared_ptr<zval> ret = MyApiTool::callMethodWithObject(*stmt, "rowCount", 0, NULL);

    if (!ret)
    {
        return 0;
    }

    return Z_LVAL_P(ret.get());
}

bool Model::add(std::string tableName, zval *data)
{
    SqlParser parser;
    parser.setPlaceholder(true);
    parser.table(tableName);

    zval placeholder;
    zval placeholderData;
    array_init(&placeholder);
    array_init(&placeholderData);

    std::shared_ptr<zval> placeholderPtr(&placeholder, [](zval *ptr){
        zval_ptr_dtor(ptr);
    });

    std::shared_ptr<zval> placeholderDataPtr(&placeholderData, [](zval *ptr){
        zval_ptr_dtor(ptr);
    });

    bool isSuccess = this->placeHolderKeyValues(data, &placeholder, &placeholderData);

    if (!isSuccess)
    {
        return false;
    }

    parser.keyValues(&placeholder);

    if (parser.isParseError())
    {
        return std::shared_ptr<zval>();
    }

    std::string sql = parser.getInsertSql();

    std::shared_ptr<zval> stmt_ptr = this->prepare(sql);

    if (!stmt_ptr || Z_TYPE_P(stmt_ptr.get()) == IS_FALSE)
    {
        return false;
    }

    if (!this->execute(stmt_ptr.get(), &placeholderData))
    {
        return false;
    }

    return true;
}

bool Model::update(std::string tableName, zval *data, zval *where)
{
    SqlParser parser;
    parser.setPlaceholder(true);
    parser.table(tableName);

    zval placeholder;
    zval placeholderData;
    zval placeholderWhere;
    array_init(&placeholder);
    array_init(&placeholderData);

    std::shared_ptr<zval> placeholderPtr(&placeholder, [](zval *ptr){
        zval_ptr_dtor(ptr);
    });

    std::shared_ptr<zval> placeholderDataPtr(&placeholderData, [](zval *ptr){
        zval_ptr_dtor(ptr);
    });

    std::shared_ptr<zval> placeholderWherePtr(&placeholderWhere, [](zval *ptr){
        if (Z_TYPE_P(ptr) == IS_ARRAY)
        {
            zval_ptr_dtor(ptr);
        }
    });

    bool isSuccess = this->placeHolderKeyValues(data, &placeholder, &placeholderData);

    if (!isSuccess)
    {
        return false;
    }

    parser.keyValues(&placeholder);

    if (Z_TYPE_P(where) == IS_ARRAY)
    {
        array_init(&placeholderWhere);

        bool isSuccess = this->placeHolderWhere(where, &placeholderWhere, &placeholderData);

        if (!isSuccess)
        {
            return false;
        }

        parser.setPlaceholder(true);
        parser.where(&placeholderWhere);
    }
    else
    {
        parser.where(where);
    }

    if (parser.isParseError())
    {
        return std::shared_ptr<zval>();
    }

    std::string sql = parser.getUpdateSql();

    std::shared_ptr<zval> stmt_ptr = this->prepare(sql);

    if (!stmt_ptr || Z_TYPE_P(stmt_ptr.get()) == IS_FALSE)
    {
        return false;
    }

    if (!this->execute(stmt_ptr.get(), &placeholderData))
    {
        return false;
    }

    int affectedRow = this->rowCount(stmt_ptr.get());

    return affectedRow > 0;
}

bool Model::del(std::string tableName, zval *where)
{
    SqlParser parser;
    parser.table(tableName);

    zval placeholder;
    zval placeholderData;

    std::shared_ptr<zval> placeholderPtr(&placeholder, [](zval *ptr){
        if (Z_TYPE_P(ptr) == IS_ARRAY)
        {
            zval_ptr_dtor(ptr);
        }
    });

    std::shared_ptr<zval> placeholderDataPtr(&placeholderData, [](zval *ptr){
        if (Z_TYPE_P(ptr) == IS_ARRAY)
        {
            zval_ptr_dtor(ptr);
        }
    });

    if (where)
    {
        if (Z_TYPE_P(where) == IS_ARRAY)
        {
            array_init(&placeholder);
            array_init(&placeholderData);

            bool isSuccess = this->placeHolderWhere(where, &placeholder, &placeholderData);

            if (!isSuccess)
            {
                return false;
            }

            parser.setPlaceholder(true);
            parser.where(&placeholder);
        }
        else
        {
            parser.where(where);
        }
    }
    else
    {
        return false;
    }

    if (parser.isParseError())
    {
        return std::shared_ptr<zval>();
    }

    std::string sql = parser.getDeleteSql();

    std::shared_ptr<zval> stmt_ptr = this->prepare(sql);

    if (!stmt_ptr || Z_TYPE_P(stmt_ptr.get()) == IS_FALSE)
    {
        return false;
    }

    if (!this->execute(stmt_ptr.get(), (Z_TYPE(placeholderData) == IS_ARRAY ? &placeholderData : NULL)))
    {
        return false;
    }

    int affectedRow = this->rowCount(stmt_ptr.get());

    return affectedRow > 0;
}

std::shared_ptr<zval> Model::get(std::string tableName, zval *where, zval *field, zval *orderBy, zval *groupBy, zval *having, std::string fetchType, std::string className)
{
    SqlParser parser;
    parser.table(tableName);
    zval placeholder;
    zval placeholderData;

    std::shared_ptr<zval> placeholderPtr(&placeholder, [](zval *ptr){
        if (Z_TYPE_P(ptr) == IS_ARRAY)
        {
            zval_ptr_dtor(ptr);
        }
    });

    std::shared_ptr<zval> placeholderDataPtr(&placeholderData, [](zval *ptr){
        if (Z_TYPE_P(ptr) == IS_ARRAY)
        {
            zval_ptr_dtor(ptr);
        }
    });

    if (where && Z_TYPE_P(where) != IS_NULL)
    {
        if (Z_TYPE_P(where) == IS_ARRAY)
        {
            array_init(&placeholder);
            array_init(&placeholderData);

            bool isSuccess = this->placeHolderWhere(where, &placeholder, &placeholderData);

            if (!isSuccess)
            {
                return std::shared_ptr<zval>();
            }

            parser.setPlaceholder(true);
            parser.where(&placeholder);
        }
        else
        {
            parser.where(where);
        }
    }

    if (field && Z_TYPE_P(field) != IS_NULL)
    {
        parser.select(field);
    }

    if (orderBy && Z_TYPE_P(orderBy) != IS_NULL)
    {
        parser.orderBy(orderBy);
    }

    if (groupBy && Z_TYPE_P(groupBy) != IS_NULL)
    {
        parser.groupBy(groupBy);
    }

    if (having && Z_TYPE_P(having) != IS_NULL)
    {
        parser.having(having);
    }

    if (parser.isParseError())
    {
        return std::shared_ptr<zval>();
    }

    std::string sql = parser.getQuerySql();

    std::shared_ptr<zval> stmt_ptr = this->prepare(sql);

    if (!stmt_ptr || Z_TYPE_P(stmt_ptr.get()) == IS_FALSE)
    {
        return std::shared_ptr<zval>();
    }

    if (!this->execute(stmt_ptr.get(), (Z_TYPE(placeholderData) == IS_ARRAY ? &placeholderData : NULL)))
    {
        return std::shared_ptr<zval>();
    }

    std::shared_ptr<zval> ret;

    if (fetchType == "fetchAll")
    {
        ret = this->fetchAll(stmt_ptr.get());
    }
    else if (fetchType == "fetchOne")
    {
        ret = this->fetchOne(stmt_ptr.get());
    }
    else if (fetchType == "fetchObjects")
    {
        ret = this->fetchObjects(stmt_ptr.get(), className);
    }
    else if (fetchType == "fetchObject")
    {
        ret = this->fetchObject(stmt_ptr.get(), className);
    }

    return ret;
}

Model Model::getInstance()
{
    static Model model;

    return model;
}

bool Model::placeHolderKeyValues(zval *data, zval *placeholderKeyValues, zval *placeholderData)
{
    zend_string *key;
    zval *value;

    ZEND_HASH_FOREACH_STR_KEY_VAL(Z_ARRVAL_P(data), key, value){
        if (key == NULL || (Z_TYPE_P(value) < IS_LONG || Z_TYPE_P(value) > IS_STRING))
        {
            return false;
        }

        std::string ph_str = std::string(":") + ZSTR_VAL(key);
        add_assoc_string(placeholderKeyValues, ZSTR_VAL(key), ph_str.c_str());

        switch (Z_TYPE_P(value))
        {
            case IS_LONG:
                add_assoc_long(placeholderData, ph_str.c_str(), Z_LVAL_P(value));
                break;
            case IS_DOUBLE:
                add_assoc_double(placeholderData, ph_str.c_str(), Z_DVAL_P(value));
                break;
            case IS_STRING:
                add_assoc_string(placeholderData, ph_str.c_str(), Z_STRVAL_P(value));
                break;
        }

    }ZEND_HASH_FOREACH_END();

    return true;
}

bool Model::placeHolderWhere(zval *where, zval *placeholderWhere, zval* placeholderData)
{
    if (!where || Z_TYPE_P(where) != IS_ARRAY)
    {
        return false;
    }

    zend_string *key;
    zval *value;

    ZEND_HASH_FOREACH_STR_KEY_VAL(Z_ARRVAL_P(where), key, value){
        if (key == NULL || (Z_TYPE_P(value) < IS_LONG || Z_TYPE_P(value) > IS_ARRAY))
        {
            return false;
        }

        std::string ph_str;
        if (Z_TYPE_P(value) != IS_ARRAY)
        {
            ph_str = std::string(":") + ZSTR_VAL(key);
            add_assoc_string(placeholderWhere, ZSTR_VAL(key), ph_str.c_str());
        }

        switch (Z_TYPE_P(value))
        {
            case IS_LONG:
                add_assoc_long(placeholderData, ph_str.c_str(), Z_LVAL_P(value));
                break;
            case IS_DOUBLE:
                add_assoc_double(placeholderData, ph_str.c_str(), Z_DVAL_P(value));
                break;
            case IS_STRING:
                add_assoc_string(placeholderData, ph_str.c_str(), Z_STRVAL_P(value));
                break;
            case IS_ARRAY:
                zval *v;
                zval inArray;
                array_init(&inArray);

                ZEND_HASH_FOREACH_VAL(Z_ARRVAL_P(value), v)
                    if (Z_TYPE_P(v) < IS_LONG || Z_TYPE_P(v) > IS_STRING)
                    {
                        return false;
                    }

                    if (Z_TYPE_P(v) == IS_STRING)
                    {
                        std::string tmp_str = std::string(":") + Z_STRVAL_P(v);
                        add_next_index_string(&inArray, tmp_str.c_str());
                        add_assoc_string(placeholderData, tmp_str.c_str(), Z_STRVAL_P(v));
                    }
                    else
                    {
                        add_next_index_zval(&inArray, v);
                    }
                ZEND_HASH_FOREACH_END();

                add_assoc_zval(placeholderWhere, ZSTR_VAL(key), &inArray);
                break;
        }

    }ZEND_HASH_FOREACH_END();

    return true;
}
