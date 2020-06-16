#ifndef MYAPI_ROUTER_EX_H
#define MYAPI_ROUTER_EX_H

#include <string>
#include <memory>

class Router
{
public:
    std::string getApi();
    std::string getModule();
    std::string getAction();
    std::string getController();
    std::string getVersionClass();
    void init();

    static Router getInstance();
};

#endif //MYAPI_ROUTER_EX_H
