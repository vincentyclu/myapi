#ifndef MYAPI_H
#define MYAPI_H

#include <vector>
#include <string>

extern "C" {
    #include "php.h"
}

class MyApiTool
{
public:
    MyApiTool();
    ~MyApiTool();
    MyApiTool(std::string className);
    int getNum();

    static zval callFunction(const char *function_name, std::vector<zval> vec);
    static zval callMethod(const char* class_name, const char *function_name, std::vector<zval> vec);
    static zval callMethodWithObject(zval& obj, const char *function_name, std::vector<zval> vec);
    zval callMethod(const char *function_name, std::vector<zval> vec);
    static zval* getData();

    static zval getLong(long num);
    static zval getString(std::string str);
    static zval getString(zend_string* str);

private:
    zval object;
};

#endif

