#ifndef MYAPI_REQUEST_EX_H
#define MYAPI_REQUEST_EX_H

#include <string>
#include <memory>

class Request
{
public:
    void init();
    zval* get(std::string key);
    zval* post(std::string key);
    zval* server(std::string key);
    zval* file(std::string key);
    zval* request(std::string key);
    zval* argv(zend_long key);

    static Request getInstance();
};

#endif //MYAPI_REQUEST_EX_H
