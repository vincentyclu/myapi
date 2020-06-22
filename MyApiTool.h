#ifndef MYAPI_H
#define MYAPI_H

#include <vector>
#include <string>

extern "C" {
    #include "php.h"
}
#include <memory>
#include <functional>

class MyApiTool
{
public:
    MyApiTool();
    ~MyApiTool();
    MyApiTool(std::string className);
    bool isCallable();
    zval getObject();

    static std::shared_ptr<zval> callFunction(const char *function_name, int params_count = 0, zval params[] = NULL);
    static std::shared_ptr<zval> callMethod(const char* class_name, const char *function_name, int params_count = 0, zval params[] = NULL);
    static std::shared_ptr<zval> callMethodWithObject(zval& obj, const char *function_name, int params_count = 0, zval params[] = NULL);
    std::shared_ptr<zval> callMethod(const char *function_name, int params_count = 0, zval params[] = NULL);

    static zval* getData();

    static void throwException(std::string msg, zend_long code);
    static void throwException(const char * msg, zend_long code);

    static zval getLong(long num);
    static zval getString(std::string str);
    static zval getString(zend_string* str);
    static std::shared_ptr<zend_string> getZendString(const char* str, bool isDelete = true);
    static std::shared_ptr<zval> getContant(const char* str, bool isDelete = true);
    static std::shared_ptr<zval> getContant(const char* str, zend_class_entry *entry, bool isDelete = true);
    static zval* getZvalByHashTable(HashTable *ht, const char* str);
    static zval* getZvalByHashTable(HashTable *ht, zend_string* str);
    static zval* getZvalByHashTable(HashTable *ht, zend_long index);
    static zval* getZvalByHashTableEx(HashTable *ht, const char* key);
    static std::shared_ptr<zval> getZval(zval *valPtr, std::function<void ()> initFun, bool isDelete = true);


private:
    zval object;
    bool callable_tag = false;

};

#endif

