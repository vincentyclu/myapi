#ifndef MYAPI_MODEL_EX_H
#define MYAPI_MODEL_EX_H

#include <string>
#include <memory>
#include <vector>

class Model
{
public:
    void init();
    std::shared_ptr<zval> query(std::string sql);
    int exec(std::string sql);
    std::shared_ptr<zval> prepare(std::string sql);
    bool execute(zval*stmt, zval *param);
    std::shared_ptr<zval> fetchAll(zval*stmt);
    std::shared_ptr<zval> fetchOne(zval*stmt);
    std::shared_ptr<zval> fetchObjects(zval*stmt, std::string className);
    std::shared_ptr<zval> fetchObject(zval*stmt, std::string className);
    int rowCount(zval*stmt);

    bool add(std::string tableName, zval *data);
    bool update(std::string tableName, zval *data, zval *where);
    bool del(std::string tableName, zval *where);
    std::shared_ptr<zval> get(std::string tableName, zval *where, zval *field, zval *orderBy, zval *groupBy, zval *having, std::string fetchType = "fetchAll", std::string className = "");

    static Model getInstance();

private:
    bool placeHolderKeyValues(zval *data, zval *placeholderKeyValues, zval *placeholderData);
    bool placeHolderWhere(zval *where, zval *placeholderWhere, zval* placeholderData);
};

#endif //MYAPI_MODEL_EX_H

