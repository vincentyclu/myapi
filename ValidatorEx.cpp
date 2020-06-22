extern "C" {
    #ifdef HAVE_CONFIG_H
    # include "config.h"
    #endif

    #include "php.h"
    #include "ext/standard/info.h"
    #include "ext/standard/php_var.h"
}
#include "MyApiTool.h"
#include "ValidatorEx.h"
#include "zend_exceptions.h"
#include "ResponseEx.h"
#include "error_code.h"
#include "ConfigEx.h"


Validator::Validator()
{
}

bool Validator::checkInt(zval *data)
{
    if (!data)
    {
        return false;
    }

    if (Z_TYPE_P(data) != IS_LONG)
    {
        MYAPI_ERR_RESPONSE(201, ZSTR_VAL(this->key));
        return false;
    }

    zval *minLaw = MyApiTool::getZvalByHashTable(Z_ARRVAL_P(law), "min");

    if (!minLaw || Z_TYPE_P(minLaw) != IS_LONG)
    {
        MYAPI_ERR_RESPONSE(202, ZSTR_VAL(this->key));
        return false;
    }

    if (minLaw && Z_LVAL_P(data) < Z_LVAL_P(minLaw))
    {
        MYAPI_ERR_RESPONSE(203, ZSTR_VAL(this->key));
        return false;
    }

    zval *maxLaw = MyApiTool::getZvalByHashTable(Z_ARRVAL_P(law), "max");

    if (!maxLaw || Z_TYPE_P(maxLaw) != IS_LONG)
    {
        MYAPI_ERR_RESPONSE(202, ZSTR_VAL(this->key));
        return false;
    }

    if (maxLaw && Z_LVAL_P(data) > Z_LVAL_P(maxLaw))
    {
        MYAPI_ERR_RESPONSE(204, ZSTR_VAL(this->key));
        return false;
    }

    result = data;

    return true;
}

bool Validator::checkDouble(zval *data)
{
    if (!data)
    {
        return false;
    }

    if (Z_TYPE_P(data) != IS_DOUBLE)
    {
        MYAPI_ERR_RESPONSE(205, ZSTR_VAL(this->key));
        return false;
    }

    zval *minLaw = MyApiTool::getZvalByHashTable(Z_ARRVAL_P(law), "min");

    if (!minLaw || Z_TYPE_P(minLaw) != IS_DOUBLE)
    {
        MYAPI_ERR_RESPONSE(202, ZSTR_VAL(this->key));
        return false;
    }

    if (minLaw && Z_DVAL_P(data) < Z_DVAL_P(minLaw))
    {
        MYAPI_ERR_RESPONSE(203, ZSTR_VAL(this->key));
        return false;
    }

    zval* maxLaw = MyApiTool::getZvalByHashTable(Z_ARRVAL_P(law), "max");

    if (!maxLaw || Z_TYPE_P(maxLaw) != IS_DOUBLE)
    {
        MYAPI_ERR_RESPONSE(202, ZSTR_VAL(this->key));
        return false;
    }

    if (maxLaw && Z_DVAL_P(data) > Z_DVAL_P(maxLaw))
    {
        MYAPI_ERR_RESPONSE(204, ZSTR_VAL(this->key));
        return false;
    }

    result = data;

    return true;
}

bool Validator::checkString(zval *data)
{
    if (!data)
    {
        return false;
    }

    if (Z_TYPE_P(data) != IS_STRING)
    {
        MYAPI_ERR_RESPONSE(206, ZSTR_VAL(this->key));
        return false;
    }

    zval *minLaw = MyApiTool::getZvalByHashTable(Z_ARRVAL_P(law), "min_len");

    if (!minLaw || Z_TYPE_P(minLaw) != IS_LONG)
    {
        MYAPI_ERR_RESPONSE(202, ZSTR_VAL(this->key));
        return false;
    }

    if (minLaw && Z_STRLEN_P(data) < Z_LVAL_P(minLaw))
    {
        MYAPI_ERR_RESPONSE(207, ZSTR_VAL(this->key));
        return false;
    }

    zval *maxLaw = MyApiTool::getZvalByHashTable(Z_ARRVAL_P(law), "max_len");

    if (!maxLaw || Z_TYPE_P(maxLaw) != IS_LONG)
    {
        MYAPI_ERR_RESPONSE(202, ZSTR_VAL(this->key));
        return false;
    }

    if (maxLaw && Z_STRLEN_P(data) > Z_LVAL_P(maxLaw))
    {
        MYAPI_ERR_RESPONSE(208, ZSTR_VAL(this->key));
        return false;
    }

    result = data;

    return true;
}

bool Validator::checkDate(zval *data)
{
    if (!data)
    {
        return false;
    }

    if (Z_TYPE_P(data) != IS_STRING)
    {
        MYAPI_ERR_RESPONSE(209, ZSTR_VAL(this->key));
        return false;
    }

    zval param;

    std::shared_ptr<zval> ptr = MyApiTool::getZval(&param, [&param, &data](){
        ZVAL_STRING(&param, Z_STRVAL_P(data));
    });

    zval params[] = {*ptr};

    std::shared_ptr<zval> retPtr = MyApiTool::callFunction("strtotime", 1, params);

    if (Z_TYPE(*retPtr) == IS_FALSE)
    {
        MYAPI_ERR_RESPONSE(209, ZSTR_VAL(this->key));
        return false;
    }

    zval* dataPtr = MyApiTool::getZvalByHashTable(Z_ARRVAL_P(law), "data");

    if (dataPtr && Z_TYPE_P(dataPtr) == IS_STRING && std::string(Z_STRVAL_P(dataPtr)) == "timestamp")
    {
        convert_to_long(data);
        Z_LVAL_P(data) = Z_LVAL_P(retPtr.get());
    }

    result = data;

    return true;
}

bool Validator::checkFile(zval *data)
{
    if (!data)
    {
        return false;
    }

    if (Z_TYPE_P(data) != IS_ARRAY)
    {
        MYAPI_ERR_RESPONSE(210, ZSTR_VAL(this->key));
        return false;
    }

    zval* lawSizePtr = MyApiTool::getZvalByHashTable(Z_ARRVAL_P(law), "size");

    if (lawSizePtr)
    {
        if (!lawSizePtr || Z_TYPE(*lawSizePtr) != IS_LONG)
        {
            MYAPI_ERR_RESPONSE(202, ZSTR_VAL(this->key));
            return false;
        }

        zval* sizePtr = MyApiTool::getZvalByHashTable(Z_ARRVAL_P(data), "size");

        if (!sizePtr || Z_LVAL(*sizePtr) > Z_LVAL(*lawSizePtr))
        {
            MYAPI_ERR_RESPONSE(211, ZSTR_VAL(this->key));
            return false;
        }
    }

    zval* lawTypePtr = MyApiTool::getZvalByHashTable(Z_ARRVAL_P(law), "file_type");

    if (lawTypePtr)
    {
        if (!lawTypePtr || Z_TYPE(*lawTypePtr) != IS_ARRAY)
        {
            MYAPI_ERR_RESPONSE(202, ZSTR_VAL(this->key));
            return false;
        }

        zval* typePtr = MyApiTool::getZvalByHashTable(Z_ARRVAL_P(data), "type");

        zval *val;
        bool isTypeValid = false;
        std::string currentFileType = Z_STRVAL(*typePtr);

        ZEND_HASH_FOREACH_VAL(Z_ARRVAL_P(lawTypePtr), val)
            std::string lawFileType = Z_STRVAL_P(val);

            if (currentFileType == lawFileType)
            {
                isTypeValid = true;
                break;
            }
        ZEND_HASH_FOREACH_END();

        if (!isTypeValid)
        {
            MYAPI_ERR_RESPONSE(212, ZSTR_VAL(this->key));
            return false;
        }
    }

    result = data;

    return true;
}

bool Validator::checkRegex(zval *data)
{
    if (!data)
    {
        return false;
    }

    if (Z_TYPE_P(data) != IS_STRING)
    {
        MYAPI_ERR_RESPONSE(213, ZSTR_VAL(this->key));
        return false;
    }

    zval *regexLaw = MyApiTool::getZvalByHashTable(Z_ARRVAL_P(law), "pattern");

    if (!regexLaw || Z_TYPE_P(regexLaw) != IS_STRING)
    {
        MYAPI_ERR_RESPONSE(202, ZSTR_VAL(this->key));
        return false;
    }

    zval strParam;

    std::shared_ptr<zval> dataPtr = MyApiTool::getZval(&strParam, [&strParam, &data](){
        ZVAL_STRING(&strParam, Z_STRVAL_P(data));
    });

    zval params[] = {*regexLaw, *dataPtr};

    std::shared_ptr<zval> retPtr = MyApiTool::callFunction("preg_match", 2, params);

    if (Z_TYPE(*retPtr) == IS_FALSE || (Z_TYPE(*retPtr) == IS_LONG && Z_LVAL(*retPtr) == 0))
    {
        MYAPI_ERR_RESPONSE(214, ZSTR_VAL(this->key));
        return false;
    }

    result = data;

    return true;
}

bool Validator::doCheck()
{
    if (Z_TYPE_P(law) != IS_ARRAY)
    {
        MYAPI_ERR_RESPONSE(202, ZSTR_VAL(this->key));
        return false;
    }

    if (this->key == NULL)
    {
        Response::getInstance().setErrorResult(MYAPI_ERR(218));
        return false;
    }

    zval* sourcePtr = MyApiTool::getZvalByHashTable(Z_ARRVAL_P(law), "source");

    if (!sourcePtr)
    {
        MYAPI_ERR_RESPONSE(202, ZSTR_VAL(this->key));
        return false;
    }

    std::string source = Z_STRVAL_P(sourcePtr);

    zval *data;

    if (this->key && source == "get")
    {
        data = MyApiTool::getZvalByHashTable(Z_ARRVAL_P(this->getData), this->key);
    }
    else if (this->key && source == "post")
    {
        data = MyApiTool::getZvalByHashTable(Z_ARRVAL_P(this->postData), this->key);
    }
    else if (this->key && source == "file" && this->fileData && Z_TYPE_P(this->fileData) == IS_ARRAY)
    {
        data = MyApiTool::getZvalByHashTable(Z_ARRVAL_P(this->fileData), this->key);
    }
    else if (source == "cli" && this->cliData && Z_TYPE_P(this->cliData) == IS_ARRAY)
    {
        zval* indexPtr = MyApiTool::getZvalByHashTable(Z_ARRVAL_P(law), "index");

        if (!indexPtr || Z_TYPE_P(indexPtr) != IS_LONG)
        {
            MYAPI_ERR_RESPONSE(202, ZSTR_VAL(this->key));
            return false;
        }

        data = MyApiTool::getZvalByHashTable(Z_ARRVAL_P(this->cliData), Z_LVAL_P(indexPtr));

        result = data;
        return true;
    } else {
        MYAPI_ERR_RESPONSE(215, ZSTR_VAL(this->key));
        return false;
    }

    zval* requirePtr = MyApiTool::getZvalByHashTable(Z_ARRVAL_P(law), "require");

    if (requirePtr)
    {
        if (Z_TYPE_P(requirePtr) == IS_TRUE && !data)
        {
            MYAPI_ERR_RESPONSE(216, ZSTR_VAL(this->key));
            return false;
        }
    }

    if (!data)
    {
        result = NULL;
        return true;
    }

    bool ret;

    zval* typePtr = MyApiTool::getZvalByHashTable(Z_ARRVAL_P(law), "type");

    if (!typePtr)
    {
        MYAPI_ERR_RESPONSE(202, ZSTR_VAL(this->key));
        return false;
    }

    std::string type = Z_STRVAL_P(typePtr);

    if (type == "int")
    {
        ret = checkInt(data);
    }
    else if (type == "float")
    {
        ret = checkDouble(data);
    }
    else if (type == "string")
    {
        ret = checkString(data);
    }
    else if (type == "date")
    {
        ret = checkDate(data);
    }
    else if (type == "file")
    {
        ret = checkFile(data);
    }
    else if (type == "regex")
    {
        ret = checkRegex(data);
    }
    else
    {
        MYAPI_ERR_RESPONSE(217, ZSTR_VAL(this->key));
        return false;
    }

    return ret;
}

void Validator::setLaw(zval *law)
{
    this->law = law;
}

void Validator::setGetData(zval *getData)
{
    this->getData = getData;
}

void Validator::setPostData(zval *postData)
{
    this->postData = postData;
}

void Validator::setFileData(zval *fileData)
{
    this->fileData = fileData;
}

void Validator::setCliData(zval *cliData)
{
    this->cliData = cliData;
}

zval *Validator::getResult()
{
    return this->result;
}

void Validator::setKey(zend_string *key)
{
    this->key = key;
}

void Validator::setNumKey(zend_long numKey)
{
    this->numKey = numKey;
}

std::string Validator::getErrorMsg()
{
    return error_msg;
}

int Validator::getErrorCode()
{
    return error_code;
}

void Validator::throwError(std::string msg, int code)
{
    error_msg = msg;

    error_code = code;

    Response::getInstance().setErrorResult(msg, code);
}

bool Validator::doValidator(zval *laws, zval *get_data, zval *post_data, zval *file_data, zval *cli_data, zval *result)
{
    zend_string *key;
    zend_long num_key;
    zval *val;
    Validator myApiValidator;

    ZEND_HASH_FOREACH_KEY_VAL(Z_ARRVAL_P(laws), num_key, key, val)
        myApiValidator.setNumKey(num_key);
        myApiValidator.setKey(key);
        myApiValidator.setLaw(val);
        myApiValidator.setPostData(post_data);
        myApiValidator.setGetData(get_data);
        myApiValidator.setFileData(file_data);
        myApiValidator.setCliData(cli_data);

        bool isSuccess = myApiValidator.doCheck();

        if (!isSuccess)
        {
            return false;
        }

        zval *ret = myApiValidator.getResult();

        if (ret)
        {
            add_assoc_zval(result, ZSTR_VAL(key), ret);
        }
        else
        {
            add_assoc_null(result, ZSTR_VAL(key));
        }


    ZEND_HASH_FOREACH_END();

    return true;
}
