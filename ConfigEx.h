#ifndef MYAPI_CONFIG_EX_H
#define MYAPI_CONFIG_EX_H

#include <string>
#include <memory>

class Config
{
public:
    std::shared_ptr<zval> get(std::string file, bool isEnv = false);
    zval* getConfig(std::string file, std::string key, bool isEnv=false);
    std::string getLang(std::string key, std::string lang = "");
    std::string getErrorMsg(int code);
    static Config getInstance();
};

#endif //MYAPI_CONFIG_EX_H
