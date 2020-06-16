#ifndef MYAPI_SQL_PARSER_H
#define MYAPI_SQL_PARSER_H

#include <string>
#include <memory>

class SqlParser
{
public:
    SqlParser();
    SqlParser& select(std::string selectField = "*");
    SqlParser& select(zval *selectField);

    SqlParser& table(std::string tableName);

    SqlParser& where(std::string where);
    SqlParser& andWhere(std::string where);
    SqlParser& orWhere(std::string where);

    SqlParser& where(zval *where);
    SqlParser& andWhere(zval *where);
    SqlParser& orWhere(zval *where);
    SqlParser& whereOr(zval *where);
    SqlParser& andWhereOr(zval *where);
    SqlParser& orWhereOr(zval *where);

    SqlParser& orderBy(std::string orderByField);
    SqlParser& orderBy(zval *orderByField);
    SqlParser& groupBy(std::string groupByField);
    SqlParser& groupBy(zval *groupByField);

    SqlParser& having(std::string havingField);

    SqlParser& having(zval *havingField);
    SqlParser& havingOr(zval *havingField);

    std::string getQuerySql();
    std::string getWhereSql();
    bool isParenthesis(std::string str);
    bool isParseError();

    SqlParser& keys(std::string keys);
    SqlParser& values(std::string values);
    SqlParser& keyValues(std::string keyValues);
    SqlParser& valuesAll(std::string valuesAll);

    SqlParser& keyValues(zval *keyValues);
    SqlParser& valuesAll(zval *valuesAll);

    std::string getDeleteSql();
    std::string getInsertSql();
    std::string getUpdateSql();
    std::string getInsertAllSql();
    std::string getString(zval *val, bool quotation = true);

    SqlParser& setPlaceholder(bool placeholder);

private:
    std::string getWhereSqlByZval(zval *where, std::string type="AND");

private:
    std::string _select;
    std::string _tableName;
    std::string _where;
    std::string _orderBy;
    std::string _groupBy;
    std::string _having;
    std::string _keys;
    std::string _values;
    std::string _keyValues;
    std::string _valuesAll;
    bool _isParseError = false;
    bool _placeholder = false;
};

#endif //MYAPI_ROUTER_EX_H

