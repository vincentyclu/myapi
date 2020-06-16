extern "C" {
    #ifdef HAVE_CONFIG_H
    # include "config.h"
    #endif

    #include "php.h"
    #include "ext/standard/info.h"
    #include "ext/standard/php_var.h"
    #include "ext/json/php_json.h"
    #include "zend_smart_str.h"
    #include "zend_string.h"
    #include "ext/standard/php_string.h"
}
#include "php_myapi.h"
#include "MyApiTool.h"
#include "SqlParser.h"
#include <sstream>

SqlParser::SqlParser()
{

}

SqlParser& SqlParser::select(std::string selectField)
{
    _select = selectField;

    return *this;
}
SqlParser& SqlParser::select(zval *selectField)
{
    if (Z_TYPE_P(selectField) != IS_STRING && Z_TYPE_P(selectField) != IS_ARRAY)
    {
        php_error_docref(NULL, E_WARNING, "select field error");
        _isParseError = true;
        return *this;
    }

    if (Z_TYPE_P(selectField) == IS_STRING)
    {
        return this->select(Z_STRVAL_P(selectField));
    }

    zval *val;
    ZEND_HASH_FOREACH_VAL(Z_ARRVAL_P(selectField), val)
        if (Z_TYPE_P(val) != IS_STRING)
        {
            php_error_docref(NULL, E_WARNING, "The value of array must be a string");
            _isParseError = true;
            continue;
        }
        _select += _select.length() != 0 ? "," : "";
        _select += std::string("`") + Z_STRVAL_P(val) + "`";
    ZEND_HASH_FOREACH_END();

    return *this;
}

SqlParser& SqlParser::table(std::string tableName)
{
    _tableName = "`" + tableName + "`";

    return *this;
}
SqlParser& SqlParser::where(std::string where)
{
    if (_where.length() != 0)
    {
        return *this;
    }

    _where += where;

    return *this;
}

SqlParser& SqlParser::andWhere(std::string where)
{
    if (where.length() == 0)
    {
        return *this;
    }

    bool isAddParenthesis = isParenthesis(_where);
    _where = (isAddParenthesis ? "(" : "") + _where + (isAddParenthesis ? ")" : "");

    bool isAddParentthesisNew = isParenthesis(where);
    _where += std::string(" AND ") + (isAddParentthesisNew ? "(" : "") + where + (isAddParentthesisNew ? ")" : "");

    return *this;
}

SqlParser& SqlParser::orWhere(std::string where)
{
    if (where.length() == 0)
    {
        return *this;
    }

    bool isAddParenthesis = isParenthesis(_where);
    _where = (isAddParenthesis ? "(" : "") + _where + (isAddParenthesis ? ")" : "");

    bool isAddParentthesisNew = isParenthesis(where);
    _where += std::string(" OR ") + (isAddParentthesisNew ? "(" : "") + where + (isAddParentthesisNew ? ")" : "");

    return *this;
}

SqlParser& SqlParser::where(zval *where)
{
    if (!where)
    {
        php_error_docref(NULL, E_WARNING, "where should not be empty");
        _isParseError = true;
        return *this;
    }

    std::string whereSql = this->getWhereSqlByZval(where);

    return this->where(whereSql);
}

SqlParser& SqlParser::andWhere(zval *where)
{
    if (!where)
    {
        php_error_docref(NULL, E_WARNING, "where should not be empty");
        _isParseError = true;
        return *this;
    }

    std::string whereSql = this->getWhereSqlByZval(where);
    return this->andWhere(whereSql);
}

SqlParser& SqlParser::orWhere(zval *where)
{
    if (!where)
    {
        php_error_docref(NULL, E_WARNING, "where should not be empty");
        _isParseError = true;
        return *this;
    }

    std::string whereSql = this->getWhereSqlByZval(where);
    return this->orWhere(whereSql);
}


SqlParser& SqlParser::whereOr(zval *where)
{
    if (!where)
    {
        php_error_docref(NULL, E_WARNING, "where should not be empty");
        _isParseError = true;
        return *this;
    }

    std::string whereSql = this->getWhereSqlByZval(where, "OR");

    return this->where(whereSql);
}


SqlParser& SqlParser::andWhereOr(zval *where)
{
    if (!where)
    {
        php_error_docref(NULL, E_WARNING, "where should not be empty");
        _isParseError = true;
        return *this;
    }

    std::string whereSql = this->getWhereSqlByZval(where, "OR");

    return this->andWhere(whereSql);
}

SqlParser& SqlParser::orWhereOr(zval *where)
{
    if (!where)
    {
        php_error_docref(NULL, E_WARNING, "where should not be empty");
        _isParseError = true;
        return *this;
    }

    std::string whereSql = this->getWhereSqlByZval(where, "OR");

    return this->orWhere(whereSql);
}

SqlParser& SqlParser::orderBy(std::string orderByField)
{
    _orderBy = orderByField;

    return *this;
}

SqlParser& SqlParser::orderBy(zval *orderByField)
{
    if (!orderByField)
    {
        return *this;
    }

    if (Z_TYPE_P(orderByField) != IS_STRING && Z_TYPE_P(orderByField) != IS_ARRAY)
    {
        php_error_docref(NULL, E_WARNING, "orderby field should be arrays or strings");
        _isParseError = true;
        return *this;
    }

    if (Z_TYPE_P(orderByField) == IS_STRING)
    {
        return this->orderBy(Z_STRVAL_P(orderByField));
    }

    zend_string *key;
    zval *val;
    ZEND_HASH_FOREACH_STR_KEY_VAL(Z_ARRVAL_P(orderByField), key, val)
        if (key == NULL)
        {
            php_error_docref(NULL, E_WARNING, "keys of arrays should be a string");
            _isParseError = true;
            continue;
        }

        if (Z_TYPE_P(val) != IS_STRING)
        {
            php_error_docref(NULL, E_WARNING, "values of arrays should be a string");
            _isParseError = true;
            continue;
        }

        _orderBy += _orderBy.length() != 0 ? "," : "";
        _orderBy += std::string("`") + ZSTR_VAL(key) + "`" + std::string(" ") + Z_STRVAL_P(val);
    ZEND_HASH_FOREACH_END();
    return *this;
}

SqlParser& SqlParser::groupBy(std::string groupByField)
{
    _groupBy = groupByField;

    return *this;
}

SqlParser& SqlParser::groupBy(zval *groupByField)
{
    if (!groupByField)
    {
        return *this;
    }

    if (Z_TYPE_P(groupByField) != IS_STRING && Z_TYPE_P(groupByField) != IS_ARRAY)
    {
        php_error_docref(NULL, E_WARNING, "groupby field should be arrays or strings");
        _isParseError = true;
        return *this;
    }

    if (Z_TYPE_P(groupByField) == IS_STRING)
    {
        return this->groupBy(Z_STRVAL_P(groupByField));
    }

    zval *val;
    ZEND_HASH_FOREACH_VAL(Z_ARRVAL_P(groupByField), val)
        if (Z_TYPE_P(val) != IS_STRING)
        {
            php_error_docref(NULL, E_WARNING, "values of arrays should be strings");
            _isParseError = true;
            continue;
        }

        _groupBy += _groupBy.length() != 0 ? "," : "";
        _groupBy += std::string("`") + Z_STRVAL_P(val) + "`";
    ZEND_HASH_FOREACH_END();
    return *this;
}

SqlParser& SqlParser::having(std::string havingField)
{
    _having = havingField;

    return *this;
}

SqlParser& SqlParser::having(zval *havingField)
{
    if (!havingField)
    {
        return *this;
    }

    std::string whereSql = this->getWhereSqlByZval(havingField);

    return this->having(whereSql);
}

SqlParser& SqlParser::havingOr(zval *havingField)
{
    if (!havingField)
    {
        return *this;
    }

    std::string whereSql = this->getWhereSqlByZval(havingField, "OR");

    return this->having(whereSql);
}

std::string SqlParser::getQuerySql()
{
    std::stringstream ss;
    ss << "SELECT " << (_select.length() == 0 ? "*" : _select) << " FROM " << _tableName;

    if (_where.length())
    {
        ss << " WHERE " << _where;
    }

    if (_orderBy.length())
    {
        ss << " ORDER BY " << _orderBy;
    }

    if (_groupBy.length())
    {
        ss << " GROUP BY " << _groupBy;
    }

    if (_having.length())
    {
        ss << " HAVING " << _having;
    }

    return ss.str();
}

std::string SqlParser::getWhereSql()
{
    return _where;
}

bool SqlParser::isParenthesis(std::string str)
{
    std::string checkStrs[] = {"AND", "OR"};

    for (auto checkStr : checkStrs)
    {
        std::string::size_type pos = str.find(checkStr);

        if (pos != std::string::npos)
        {
            return true;
        }
    }

    return false;
}

bool SqlParser::isParseError()
{
    return this->_isParseError;
}

std::string SqlParser::getWhereSqlByZval(zval *where, std::string type)
{
    if (!where)
    {
        return "";
    }

    if (Z_TYPE_P(where) != IS_STRING && Z_TYPE_P(where) != IS_ARRAY)
    {
        php_error_docref(NULL, E_WARNING, "where should be arrays or strings");
        _isParseError = true;

        return "";
    }

    if (Z_TYPE_P(where) == IS_STRING)
    {
        return Z_STRVAL_P(where);
    }


    zend_string *key;
    zval *val;

    std::string whereSql;

    ZEND_HASH_FOREACH_STR_KEY_VAL(Z_ARRVAL_P(where), key, val)
        if (key == NULL)
        {
            php_error_docref(NULL, E_WARNING, "keys of arrays should be strings");
            _isParseError = true;
            continue;
        }

        whereSql += whereSql.length() == 0 ? "" : " " + type + " ";

        if (Z_TYPE_P(val) == IS_ARRAY)
        {
            zval *v;
            std::string inStr;
            ZEND_HASH_FOREACH_VAL(Z_ARRVAL_P(val), v)
                if (Z_TYPE_P(v) > 6 || Z_TYPE_P(v) < 2)
                {
                    php_error_docref(NULL, E_WARNING, "in field error");
                    _isParseError = true;
                    return "";
                }

                inStr += inStr.length() > 0 ? "," : "";
                inStr += this->getString(v, true);
            ZEND_HASH_FOREACH_END();

            if (std::string(ZSTR_VAL(key)).find(" IN") != std::string::npos || std::string(ZSTR_VAL(key)).find(" in") != std::string::npos)
            {
                whereSql += std::string(ZSTR_VAL(key)) + " (" + inStr + ")";
            }
            else
            {
                whereSql += std::string(ZSTR_VAL(key)) + " IN (" + inStr + ")";
            }

            continue;
        }

        if (std::string(ZSTR_VAL(key)).find(" BETWEEN") != std::string::npos || std::string(ZSTR_VAL(key)).find(" between") != std::string::npos)
        {
            whereSql += std::string(ZSTR_VAL(key)) + " " + this->getString(val, false);
        }
        else if (std::string(ZSTR_VAL(key)).find(" IS") != std::string::npos || std::string(ZSTR_VAL(key)).find(" is") != std::string::npos)
        {
            whereSql += std::string(ZSTR_VAL(key)) + " " + this->getString(val, false);
        }
        else if (std::string(ZSTR_VAL(key)).find(" ") != std::string::npos)
        {
            whereSql += std::string(ZSTR_VAL(key)) + " " + this->getString(val, true);
        }
        else
        {
            whereSql += std::string(ZSTR_VAL(key)) + "=" + this->getString(val, true);
        }
    ZEND_HASH_FOREACH_END();

    return whereSql;
}

SqlParser& SqlParser::keys(std::string keys)
{
    _keys = keys;

    return *this;
}

SqlParser& SqlParser::values(std::string values)
{
    _values = values;

    return *this;
}

SqlParser& SqlParser::keyValues(std::string keyValues)
{
    _keyValues = keyValues;

    return *this;
}

SqlParser& SqlParser::valuesAll(std::string valuesAll)
{
    _valuesAll = valuesAll;

    return *this;
}

SqlParser& SqlParser::keyValues(zval *keyValues)
{
    if (!keyValues || Z_TYPE_P(keyValues) != IS_ARRAY)
    {
        php_error_docref(NULL, E_WARNING, "keyValues parameters shoud be array");
        _isParseError = true;
        return *this;
    }

    zend_string *key;
    zval *val;
    ZEND_HASH_FOREACH_STR_KEY_VAL(Z_ARRVAL_P(keyValues), key, val)
        if (key == NULL)
        {
            php_error_docref(NULL, E_WARNING, "key shoud be a string");
            _isParseError = true;
            continue;
        }

        _keys += _keys.length() != 0 ? "," : "";
        _values += _values.length() != 0 ? "," : "";
        _keyValues += _keyValues.length() != 0 ? "," : "";

        _keys += std::string("`") + ZSTR_VAL(key)  +"`";
        _values += this->getString(val, true);
        _keyValues += std::string("`") + ZSTR_VAL(key) + "`=" + this->getString(val, true);
    ZEND_HASH_FOREACH_END();

    return *this;
}

SqlParser& SqlParser::valuesAll(zval *valuesAll)
{
    if (!valuesAll || Z_TYPE_P(valuesAll) != IS_ARRAY)
    {
        php_error_docref(NULL, E_WARNING, "valuesAll parameters shoud be array");
        _isParseError = true;
        return *this;
    }

    zval *val;
    bool isGetKey = true;
    ZEND_HASH_FOREACH_VAL(Z_ARRVAL_P(valuesAll), val)
        if (Z_TYPE_P(val) != IS_ARRAY)
        {
            php_error_docref(NULL, E_WARNING, "valuesAll parameters shoud be array");
            _isParseError = true;
            continue;
        }

        zend_string *k;
        zval *v;
        std::string valuesStr;

        ZEND_HASH_FOREACH_STR_KEY_VAL(Z_ARRVAL_P(val), k, v)
            if (k == NULL)
            {
                php_error_docref(NULL, E_WARNING, "key shoud be a string");
                _isParseError = true;
                continue;
            }

            if (isGetKey)
            {
                _keys += _keys.length() != 0 ? "," : "";
                _keys += std::string("`") + ZSTR_VAL(k)  +"`";
            }

            valuesStr += valuesStr.length() != 0 ? "," : "";
            valuesStr += this->getString(v, true);
        ZEND_HASH_FOREACH_END();

        _valuesAll += _valuesAll.length() != 0 ? "," : "";
        _valuesAll += std::string("(") + valuesStr + ")";
        isGetKey = false;
    ZEND_HASH_FOREACH_END();

    return *this;
}

std::string SqlParser::getDeleteSql()
{
    std::string sql = "DELETE FROM " + _tableName + " WHERE " + _where;

    return sql;
}

std::string SqlParser::getInsertSql()
{
    std::string sql = "INSERT INTO " + _tableName + "(" + _keys + ") VALUES(" + _values + ")";

    return sql;
}

std::string SqlParser::getUpdateSql()
{
    std::string sql = "UPDATE " + _tableName + " SET " + _keyValues + " WHERE " + _where;

    return sql;
}

std::string SqlParser::getInsertAllSql()
{
    std::string sql = "INSERT INTO " + _tableName + "(" + _keys + ") VALUES" + _valuesAll;

    return sql;
}

std::string SqlParser::getString(zval *val, bool quotation)
{
    std::string ret;
    switch (Z_TYPE_P(val))
    {
        case IS_LONG:
            ret = std::to_string(Z_LVAL_P(val));
            break;
        case IS_DOUBLE:
            ret = std::to_string(Z_DVAL_P(val));
            break;
        case IS_TRUE:
            ret = std::to_string(1);
            break;
        case IS_FALSE:
            ret = std::to_string(0);
            break;
        case IS_STRING:
            if (_placeholder)
            {
                ret = Z_STRVAL_P(val);
            }
            else
            {
                ret = quotation ? (std::string("'") + Z_STRVAL_P(val) + "'") : Z_STRVAL_P(val);
            }

            break;
        case IS_NULL:
            ret = "NULL";
            break;
        default:
            php_error_docref(NULL, E_WARNING, "type error");
            _isParseError = true;
            ret = "''";
            break;
    }

    return ret;
}

SqlParser& SqlParser::setPlaceholder(bool placeholder)
{
    _placeholder = placeholder;

    return *this;
}

