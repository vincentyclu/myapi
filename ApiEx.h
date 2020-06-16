#ifndef MYAPI_APIEX_H
#define MYAPI_APIEX_H

#include <string>
#include <memory>

class Api
{
public:
    std::string getApiPath();
    std::string getEnv();
    std::string getPhpSapi();
    static std::shared_ptr<zval> import(std::string filename, bool isDelete = true);

    bool isInstantiated();
    static Api getInstance();
};


#endif // MYAPI_APIEX_H
