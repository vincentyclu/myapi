#ifndef MYAPI_ORMEX_H
#define MYAPI_ORMEX_H

#include <string>
#include <memory>

class Orm
{
public:
    enum class WhereType {
        WHERE,
        AND_WHERE,
        OR_WHERE,
        WHERE_OR,
        AND_WHERE_OR,
        OR_WHERE_OR
    };

    bool save(zval *obj);
    bool update(zval *obj);
    bool del(zval *obj);
    bool updateWhereSql(zval *obj, zval *where, Orm::WhereType wt);
    std::shared_ptr<zval> get(zval *obj);
    std::shared_ptr<zval> getOne(zval *obj);

    bool initData(zval *obj, zval *data, zval *where);
    static std::string getTableName(zval *obj);
    static std::string getPrimaryKey(zval *obj);

    static Orm getInstance();
};


#endif // MYAPI_ORMEX_H
