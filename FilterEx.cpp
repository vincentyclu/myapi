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
#include "FilterEx.h"
#include "RouterEx.h"
#include "ConfigEx.h"
#include "zend_hash.h"
#include <vector>

Filter Filter::getInstance()
{
    static Filter filter;

    return filter;
}

std::vector<std::string> Filter::getFilter()
{
    std::vector<std::string> filterVector;
    std::shared_ptr<zval> sysPtr = Config::getInstance().get("sys", false);

    if (!sysPtr || Z_TYPE_P(sysPtr.get()) != IS_ARRAY)
    {
        return filterVector;
    }

    std::shared_ptr<zval> filters = MyApiTool::getZvalByHashTable(Z_ARRVAL_P(sysPtr.get()), "filter", false);

    if (filters && Z_TYPE_P(filters.get()) == IS_ARRAY)
    {
        zval *val;
        ZEND_HASH_FOREACH_VAL(Z_ARRVAL_P(filters.get()), val)
            std::shared_ptr<zval> includeRultePtr = MyApiTool::getZvalByHashTable(Z_ARRVAL_P(val), "include_rule", false);
            std::shared_ptr<zval> excludeRultePtr = MyApiTool::getZvalByHashTable(Z_ARRVAL_P(val), "exclude_rule", false);

            if (this->isDoFilter(includeRultePtr.get(), excludeRultePtr.get()))
            {
                std::shared_ptr<zval> classNamePtr = MyApiTool::getZvalByHashTable(Z_ARRVAL_P(val), "class_name", false);

                if (classNamePtr && Z_TYPE_P(classNamePtr.get()) == IS_STRING)
                {
                    filterVector.push_back(std::string(Z_STRVAL_P(classNamePtr.get())));
                }
            }
        ZEND_HASH_FOREACH_END();
    }

    return filterVector;
}

bool Filter::doBeginFilter()
{
    std::vector<std::string> filters = this->getFilter();

    if (filters.size() > 0)
    {
        for (auto class_name : filters)
        {
            MyApiTool filterTool(class_name);
            if (!filterTool.isCallable())
            {
                std::string errMsg = class_name + " FILTER NOT EXIST";
                MyApiTool::throwException(errMsg, 10);
                return false;
            }

            std::shared_ptr<zval> filterBeginPtr = filterTool.callMethod("begin");

            if (!filterBeginPtr || Z_TYPE_P(filterBeginPtr.get()) != IS_TRUE)
            {
                return false;
            }
        }
    }

    return true;
}

bool Filter::doAfterFilter(zval *actionResult)
{
    std::vector<std::string> filters = this->getFilter();

    ZVAL_MAKE_REF(actionResult);

    if (filters.size() > 0)
    {
        for (auto class_name : filters)
        {
            MyApiTool filterTool(class_name);
            if (!filterTool.isCallable())
            {
                std::string errMsg = class_name + " FILTER NOT EXIST";
                MyApiTool::throwException(errMsg, 10);

                return false;
            }

            zval params[] = {*actionResult};

            std::shared_ptr<zval> ret = filterTool.callMethod("after", 1, params);

            if (!ret || Z_TYPE_P(ret.get()) != IS_TRUE)
            {

                return false;
            }
        }
    }

    ZVAL_UNREF(actionResult);

    return true;
}

bool Filter::isDoFilter(zval *include_rule, zval *exclude_rule)
{
    std::string api = Router::getInstance().getApi();
    std::string module = Router::getInstance().getModule();
    std::string action = Router::getInstance().getAction();

    if (include_rule && Z_TYPE_P(include_rule) == IS_ARRAY && Z_ARRVAL_P(include_rule)->nNumOfElements > 0)
    {
        zval *val;
        ZEND_HASH_FOREACH_VAL(Z_ARRVAL_P(include_rule), val)
            if (Z_TYPE_P(val) != IS_STRING)
            {
                continue;
            }

            std::string tmp_rule = Z_STRVAL_P(val);

            std::string pattern1 = api + "_*_*";
            std::string pattern2 = api + "_" + module + "_*";
            std::string pattern3 = api + "_" + module + "_" + action;

            if (tmp_rule == pattern1 || tmp_rule == pattern2 || tmp_rule == pattern3)
            {
                return true;
            }

        ZEND_HASH_FOREACH_END();

        return false;
    }
    else if (exclude_rule && Z_TYPE_P(exclude_rule) == IS_ARRAY && Z_ARRVAL_P(exclude_rule)->nNumOfElements > 0)
    {
        zval *val;
        ZEND_HASH_FOREACH_VAL(Z_ARRVAL_P(exclude_rule), val)
            if (Z_TYPE_P(val) != IS_STRING)
            {
                continue;
            }

            std::string tmp_rule = Z_STRVAL_P(val);

            std::string pattern1 = api + "_*_*";
            std::string pattern2 = api + "_" + module + "_*";
            std::string pattern3 = api + "_" + module + "_" + action;

            if (tmp_rule == pattern1 || tmp_rule == pattern2 || tmp_rule == pattern3)
            {
                return false;
            }

        ZEND_HASH_FOREACH_END();

        return true;
    }

    return true;
}
