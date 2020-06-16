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
#include "VersionEx.h"

extern zend_class_entry* version_ce;

Version Version::getInstance()
{
    static Version version;

    return version;
}

void Version::setResult(zval *result)
{
    zend_update_static_property(version_ce, "result", strlen("result"), result);
}

zval* Version::getResult()
{
    zval *ret = zend_read_static_property(version_ce, "result", strlen("result"), 0);

    return ret;
}
