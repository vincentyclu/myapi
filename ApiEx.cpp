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
#include <fstream>

extern zend_class_entry *api_ce;

std::string Api::getApiPath()
{
    zval *val = zend_read_static_property(api_ce, "api_path", strlen("api_path"), 0 );

    if (val && Z_TYPE_P(val) == IS_STRING)
    {
        return Z_STRVAL_P(val);
    }

    return "";
}

std::string Api::getEnv()
{
    zval *val = zend_read_static_property(api_ce, "env", strlen("env"), 0 );

    if (val && Z_TYPE_P(val) == IS_STRING)
    {
        return Z_STRVAL_P(val);
    }

    return "";
}

std::string Api::getPhpSapi()
{
    zval *val = zend_read_static_property(api_ce, "php_sapi", strlen("php_sapi"), 0 );

    if (val && Z_TYPE_P(val) == IS_STRING)
    {
        return Z_STRVAL_P(val);
    }

    return "";
}

std::shared_ptr<zval> Api::import(std::string filename, bool isDelete)
{
    std::ifstream ifs(filename);

    if (!ifs.is_open())
    {
        php_error_docref(NULL, E_WARNING, "%s, FILE NOT EXISTING", filename.c_str());
        return std::shared_ptr<zval>();
    }

    zval fileZval;
    ZVAL_STRING(&fileZval, filename.c_str());

    zend_op_array *arr = compile_filename(ZEND_INCLUDE_ONCE, &fileZval);
    zval_ptr_dtor(&fileZval);

    if (arr == NULL)
    {
        return std::shared_ptr<zval>();
    }

    zval *ret = (zval *) emalloc(sizeof(zval));
    std::shared_ptr<zval> retPtr(ret, [isDelete](zval *ptr){
        if (isDelete)
        {
            zval_ptr_dtor(ptr);
        }

        efree(ptr);
    });

    zend_execute(arr, ret);
    destroy_op_array(arr);
    efree(arr);

    return retPtr;
}

bool Api::isInstantiated()
{
    zval *val = zend_read_static_property(api_ce, "is_instantiated", strlen("is_instantiated"), 0);

    if (Z_TYPE_P(val) == IS_TRUE)
    {
        return true;
    }

    return false;
}

Api Api::getInstance()
{
    static Api api;

    return api;
}
