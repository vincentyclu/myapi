extern "C" {
    #ifdef HAVE_CONFIG_H
    # include "config.h"
    #endif

    #include "php.h"
    #include "ext/standard/info.h"
    #include "ext/standard/php_var.h"
    #include "ext/json/php_json.h"
    #include "zend_smart_str.h"
}
#include "php_myapi.h"
#include "MyApiTool.h"
#include "ApiEx.h"
#include "ConfigEx.h"
#include <memory>

extern zend_class_entry* config_ce;

Config Config::getInstance()
{
    static Config config;

    return config;
}

std::shared_ptr<zval> Config::get(std::string file, bool isEnv)
{
    std::string apiPath = Api::getInstance().getApiPath();
    std::string filename;

    if (isEnv)
    {
        std::string env = Api::getInstance().getEnv();
        filename = apiPath + "/config/" + env + "/" + file + ".php";
    }
    else
    {
        filename = apiPath + "/config/" + file + ".php";
    }

    std::shared_ptr<zval> retPtr = Api::getInstance().import(filename);

    if (!retPtr || Z_TYPE_P(retPtr.get()) != IS_ARRAY)
    {
        return std::shared_ptr<zval>();
    }

    return retPtr;
}
