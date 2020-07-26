extern "C" {
    #ifdef HAVE_CONFIG_H
    # include "config.h"
    #endif

    #include "php.h"
    #include "ext/standard/info.h"
    #include "ext/standard/php_var.h"
    #include "ext/json/php_json.h"
    #include "SAPI.h"
    #include "ext/standard/head.h"
    #include "zend_smart_str.h"
    #include "zend_closures.h"
}
#include "php_myapi.h"
#include "MyApiTool.h"
#include "ResponseEx.h"
#include "ConfigEx.h"

extern zend_class_entry *response_ce;

bool Response::isError()
{
    zval *errorCode = zend_read_static_property(response_ce, "error_code", strlen("error_code"), 0);

    return Z_TYPE_P(errorCode) != IS_NULL;
}

void Response::header(char* line, bool rep, zend_long code)
{
    sapi_header_line ctr = {0};
    ctr.line = line;
    ctr.line_len = strlen(line);
    ctr.response_code = code;
	sapi_header_op(rep ? sapi_header_op_enum::SAPI_HEADER_REPLACE: (sapi_header_op_enum)SAPI_HEADER_ADD, &ctr);
}

void Response::setErrorResult(std::string errorMsg, int errorcode)
{
    zend_update_static_property_string(response_ce, "error_msg", strlen("error_msg"), errorMsg.c_str());
    zend_update_static_property_long(response_ce, "error_code", strlen("error_code"), errorcode);
}

void Response::setErrorResult(zval *errorMsg, int errorcode)
{
    zend_update_static_property(response_ce, "error_msg", strlen("error_msg"), errorMsg);
    zend_update_static_property_long(response_ce, "error_code", strlen("error_code"), errorcode);
}

std::string Response::getJsonString(zval *val)
{
    std::string result = "";

    if (Z_TYPE_P(val) == IS_ARRAY || Z_TYPE_P(val) == IS_OBJECT)
    {
        smart_str buf = {0};
        php_json_encode(&buf, val, 0);
        smart_str_appendc(&buf, '\0');

        result = ZSTR_VAL(buf.s);
        smart_str_free(&buf);
    }

    return result;
}

std::string Response::getXmlString(zval *data)
{
    std::string xml = "<?xml version='1.0' encoding='UTF-8'?>";
    xml += "<xml>";

    if (Z_TYPE_P(data) == IS_ARRAY)
    {
        xml += getXml(data);
    }

    xml += "</xml>";

    return xml;
}

std::string Response::getXml(zval *data)
{
    zend_string *key;
    zval *val;
    std::string xml;
    zend_long num;

    ZEND_HASH_FOREACH_KEY_VAL(Z_ARRVAL_P(data), num, key, val)
        if (key == NULL)
        {
            xml += "<list id='" + std::to_string((int)num) + "'>";
        }
        else
        {
            xml += std::string("<") + ZSTR_VAL(key) + ">";
        }

        if (Z_TYPE_P(val) == IS_ARRAY)
        {
            xml += getXml(val);
        }
        else
        {
            switch (Z_TYPE_P(val))
            {
                case IS_STRING:
                    xml += std::string(Z_STRVAL_P(val));
                    break;
                case IS_LONG:
                    xml += std::to_string(Z_LVAL_P(val));
                    break;
                case IS_DOUBLE:
                    xml += std::to_string(Z_DVAL_P(val));
                    break;
                case IS_TRUE:
                    xml += "true";
                    break;
                case IS_FALSE:
                    xml += "false";
                    break;
                default:
                    break;
            }
        }

        if (key == NULL)
        {
            xml += "</list>";
        }
        else
        {
            xml += std::string("</") + ZSTR_VAL(key) + ">";
        }

    ZEND_HASH_FOREACH_END();

    return xml;
}

zval* Response::getResult()
{
    zval *ret = zend_read_static_property(response_ce, "result", strlen("result"), 0);

    return ret;
}

void Response::setResult(zval *val)
{
    zend_update_static_property(response_ce, "result", strlen("result"), val);
}

Response Response::getInstance()
{
    static Response response;

    return response;
}

std::string Response::output()
{
    zval *isSetHeader = zend_read_static_property(response_ce, "is_set_header", strlen("is_set_header"), 0);

    if (Z_TYPE_P(isSetHeader) == IS_NULL)
    {
        //this->header("Content-type:text/json");
    }

    zval *errorCode = zend_read_static_property(response_ce, "error_code", strlen("error_code"), 0);
    zval ret;
    array_init(&ret);

    if (Z_TYPE_P(errorCode) != IS_NULL)
    {
        zval *errorMsg = zend_read_static_property(response_ce, "error_msg", strlen("error_msg"), 0);

        add_assoc_zval(&ret, "code", errorCode);

        if (Z_TYPE_P(errorMsg) == IS_STRING)
        {
            add_assoc_string(&ret, "error_msg", Z_STRVAL_P(errorMsg));
        }
        else
        {
            Z_TRY_ADDREF_P(errorMsg);
            add_assoc_zval(&ret, "err_msg", errorMsg);
        }

    }
    else
    {
        zval *result = zend_read_static_property(response_ce, "result", strlen("result"), 0);
        Z_TRY_ADDREF_P(result);
        add_assoc_zval(&ret, "result", result);
        add_assoc_long(&ret, "code", 0);
    }

    zval *outputType = Config::getInstance().getConfig("sys", "output.type", false);

    if (Z_TYPE_P(outputType) != IS_STRING)
    {
        zval_ptr_dtor(&ret);
        return "";
    }

    std::string pattern = Z_STRVAL_P(outputType);
    std::string output;

    if (pattern == "json")
    {
        output = getJsonString(&ret);
    }
    else if (pattern == "xml")
    {
        output = getXmlString(&ret);
    }
    else if (pattern == "custom")
    {
        zval *closure = Config::getInstance().getConfig("sys", "output.custom", false);

        zval params[] = {ret};
        std::shared_ptr<zval> resultPtr = MyApiTool::callClosure(closure, 1, params);

        if (!resultPtr || Z_TYPE_P(resultPtr.get()) != IS_STRING)
        {
            output = "";
        }
        else
        {
            output = Z_STRVAL(*resultPtr);
        }
    }

    zval_ptr_dtor(&ret);

    return output;
}
