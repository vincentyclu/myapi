#ifndef MYAPI_REQUEST_EX_H
#define MYAPI_REQUEST_EX_H

#include <string>
#include <memory>

class Request
{
public:
    void init();
    std::shared_ptr<zval> get(std::string key);
    std::shared_ptr<zval> post(std::string key);
    std::shared_ptr<zval> server(std::string key);
    std::shared_ptr<zval> file(std::string key);
    std::shared_ptr<zval> request(std::string key);
    std::shared_ptr<zval> argv(zend_long key);

    static Request getInstance();
};

#endif //MYAPI_REQUEST_EX_H
