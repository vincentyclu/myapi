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

zval* Config::getConfig(std::string file, std::string key, bool isEnv)
{
    zval *configObj = zend_read_static_property(config_ce, "config_obj", strlen("config_obj"), 0);

    if (!configObj || Z_TYPE_P(configObj) == IS_NULL)
    {
        zval obj;
        object_init_ex(&obj, config_ce);
        zend_update_static_property(config_ce, "config_obj", strlen("config_obj"), &obj);
        zval_ptr_dtor(&obj);
        configObj = zend_read_static_property(config_ce, "config_obj", strlen("config_obj"), 0);
    }

    std::string env = Api::getInstance().getEnv();
    std::string fileKey = isEnv ? env + "_" + file : file;

    zval* dataPtr = MyApiTool::getZvalByHashTable(Z_OBJPROP_P(configObj), fileKey.c_str());

    if (!dataPtr)
    {
        std::shared_ptr<zval> includeDataPtr = this->get(file, isEnv);

        if (!includeDataPtr)
        {
            return NULL;
        }

        zend_update_property(config_ce, configObj, fileKey.c_str(), fileKey.length(), includeDataPtr.get());
    }

    zval *data = zend_read_property(config_ce, configObj, fileKey.c_str(), fileKey.length(), 0, NULL);

    if (key == "" || Z_TYPE_P(data) != IS_ARRAY)
    {
        return data;
    }
    else
    {
        zval* vPtr = MyApiTool::getZvalByHashTableEx(Z_ARRVAL_P(data), key.c_str());

        return vPtr;
    }
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

std::string Config::getLang(std::string key, std::string lang)
{
    if (lang == "")
    {
        lang = MYAPI_G(language);
    }

    std::string file = "lang/" + lang;
    zval *val = this->getConfig(file, key, false);

    if (!val || Z_TYPE_P(val) != IS_STRING)
    {
        return "";
    }

    return Z_STRVAL_P(val);
}

std::string Config::getErrorMsg(int code)
{
    std::string key = "error_code_" + std::to_string(code);

    return getLang(key);
}
