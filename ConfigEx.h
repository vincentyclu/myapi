#ifndef MYAPI_CONFIG_EX_H
#define MYAPI_CONFIG_EX_H

#include <string>
#include <memory>

class Config
{
public:
    std::shared_ptr<zval> get(std::string file, bool isEnv = false);
    static Config getInstance();
};

#endif //MYAPI_CONFIG_EX_H
