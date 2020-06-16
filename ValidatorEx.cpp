#include "MyApiTool.h"
#include "ValidatorEx.h"
#include "zend_exceptions.h"


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
        std::string errorMsg = std::string(ZSTR_VAL(this->key)) + " NOT INTEGER";
        Validator::throwError(errorMsg, 101);
        return false;
    }

    std::shared_ptr<zval> minPtr = MyApiTool::getZvalByHashTable(Z_ARRVAL_P(law), "min", false);
    zval *minLaw = minPtr.get();

    if (!minLaw || Z_TYPE_P(minLaw) != IS_LONG)
    {
        std::string errorMsg = std::string(ZSTR_VAL(this->key)) + " LAW INVALID";
        Validator::throwError(errorMsg, 102);
        return false;
    }

    if (minPtr && Z_LVAL_P(data) < Z_LVAL_P(minPtr.get()))
    {
        std::string errorMsg = std::string(ZSTR_VAL(this->key)) + " MIN ERROR";
        Validator::throwError(errorMsg, 103);
        return false;
    }

    std::shared_ptr<zval> maxPtr = MyApiTool::getZvalByHashTable(Z_ARRVAL_P(law), "max", false);
    zval *maxLaw = maxPtr.get();

    if (!maxLaw || Z_TYPE_P(maxLaw) != IS_LONG)
    {
        std::string errorMsg = std::string(ZSTR_VAL(this->key)) + " LAW INVALID";
        Validator::throwError(errorMsg, 104);
        return false;
    }

    if (maxPtr && Z_LVAL_P(data) > Z_LVAL_P(maxPtr.get()))
    {
        std::string errorMsg = std::string(ZSTR_VAL(this->key)) + " MAX ERROR";
        Validator::throwError(errorMsg, 105);
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
        std::string errorMsg = std::string(ZSTR_VAL(this->key)) + " NOT FLOAT";
        Validator::throwError(errorMsg, 106);
        return false;
    }

    std::shared_ptr<zval> minPtr = MyApiTool::getZvalByHashTable(Z_ARRVAL_P(law), "min", false);
    zval *minLaw = minPtr.get();

    if (!minLaw || Z_TYPE_P(minLaw) != IS_DOUBLE)
    {
        std::string errorMsg = std::string(ZSTR_VAL(this->key)) + " LAW INVALID";
        Validator::throwError(errorMsg, 107);
        return false;
    }

    if (minPtr && Z_DVAL_P(data) < Z_DVAL_P(minPtr.get()))
    {
        std::string errorMsg = std::string(ZSTR_VAL(this->key)) + " MIN ERROR";
        Validator::throwError(errorMsg, 108);
        return false;
    }

    std::shared_ptr<zval> maxPtr = MyApiTool::getZvalByHashTable(Z_ARRVAL_P(law), "max", false);
    zval *maxLaw = maxPtr.get();

    if (!maxLaw || Z_TYPE_P(maxLaw) != IS_DOUBLE)
    {
        std::string errorMsg = std::string(ZSTR_VAL(this->key)) + " LAW INVALID";
        Validator::throwError(errorMsg, 109);
        return false;
    }

    if (maxPtr && Z_DVAL_P(data) > Z_DVAL_P(maxPtr.get()))
    {
        std::string errorMsg = std::string(ZSTR_VAL(this->key)) + " MAX ERROR";
        Validator::throwError(errorMsg, 110);
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
        std::string errorMsg = std::string(ZSTR_VAL(this->key)) + " NOT STRING";
        Validator::throwError(errorMsg, 111);
        return false;
    }

    std::shared_ptr<zval> minPtr = MyApiTool::getZvalByHashTable(Z_ARRVAL_P(law), "min_len", false);
    zval *minLaw = minPtr.get();

    if (!minLaw || Z_TYPE_P(minLaw) != IS_LONG)
    {
        std::string errorMsg = std::string(ZSTR_VAL(this->key)) + " LAW INVALID";
        Validator::throwError(errorMsg, 112);
        return false;
    }

    if (minPtr && Z_STRLEN_P(data) < Z_LVAL_P(minPtr.get()))
    {
        std::string errorMsg = std::string(ZSTR_VAL(this->key)) + " MIN LEN ERROR";
        Validator::throwError(errorMsg, 113);
        return false;
    }

    std::shared_ptr<zval> maxPtr = MyApiTool::getZvalByHashTable(Z_ARRVAL_P(law), "max_len", false);
    zval *maxLaw = maxPtr.get();

    if (!maxLaw || Z_TYPE_P(maxLaw) != IS_LONG)
    {
        std::string errorMsg = std::string(ZSTR_VAL(this->key)) + " LAW INVALID";
        Validator::throwError(errorMsg, 114);
        return false;
    }

    if (maxPtr && Z_STRLEN_P(data) > Z_LVAL_P(maxPtr.get()))
    {
        std::string errorMsg = std::string(ZSTR_VAL(this->key)) + " MAX LEN ERROR";
        Validator::throwError(errorMsg, 115);
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
        std::string errorMsg = std::string(ZSTR_VAL(this->key)) + " NOT DATE";
        Validator::throwError(errorMsg, 116);
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
        std::string errorMsg = std::string(ZSTR_VAL(this->key)) + " NOT DATE";
        Validator::throwError(errorMsg, 117);
        return false;
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
        std::string errorMsg = std::string(ZSTR_VAL(this->key)) + " NOT FILE";
        Validator::throwError(errorMsg, 118);
        return false;
    }

    std::shared_ptr<zval> lawSizePtr = MyApiTool::getZvalByHashTable(Z_ARRVAL_P(law), "size", false);

    if (lawSizePtr)
    {
        if (!lawSizePtr || Z_TYPE(*lawSizePtr) != IS_LONG)
        {
            std::string errorMsg = std::string(ZSTR_VAL(this->key)) + " LAW INVALID";
            Validator::throwError(errorMsg, 119);
            return false;
        }

        std::shared_ptr<zval> sizePtr = MyApiTool::getZvalByHashTable(Z_ARRVAL_P(data), "size", false);

        if (!sizePtr || Z_LVAL(*sizePtr) > Z_LVAL(*lawSizePtr))
        {
            std::string errorMsg = std::string(ZSTR_VAL(this->key)) + " SIZE INVALID";
            Validator::throwError(errorMsg, 120);
            return false;
        }
    }

    std::shared_ptr<zval> lawTypePtr = MyApiTool::getZvalByHashTable(Z_ARRVAL_P(law), "file_type", false);

    if (lawTypePtr)
    {
        if (!lawTypePtr || Z_TYPE(*lawTypePtr) != IS_ARRAY)
        {
            std::string errorMsg = std::string(ZSTR_VAL(this->key)) + " LAW INVALID";
            Validator::throwError(errorMsg, 121);
            return false;
        }

        std::shared_ptr<zval> typePtr = MyApiTool::getZvalByHashTable(Z_ARRVAL_P(data), "type", false);

        zval *val;
        bool isTypeValid = false;
        std::string currentFileType = Z_STRVAL(*typePtr);

        ZEND_HASH_FOREACH_VAL(Z_ARRVAL_P(lawTypePtr.get()), val)
            std::string lawFileType = Z_STRVAL_P(val);

            if (currentFileType == lawFileType)
            {
                isTypeValid = true;
                break;
            }
        ZEND_HASH_FOREACH_END();

        if (!isTypeValid)
        {
            std::string errorMsg = std::string(ZSTR_VAL(this->key)) + " FILE TYPE INVALID";
            Validator::throwError(errorMsg, 122);
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
        std::string errorMsg = std::string(ZSTR_VAL(this->key)) + " NOT REGEX";
        Validator::throwError(errorMsg, 123);
        return false;
    }

    std::shared_ptr<zval> regexPtr = MyApiTool::getZvalByHashTable(Z_ARRVAL_P(law), "pattern", false);
    zval *regexLaw = regexPtr.get();

    if (!regexLaw || Z_TYPE_P(regexLaw) != IS_STRING)
    {
        std::string errorMsg = std::string(ZSTR_VAL(this->key)) + " LAW INVALID";
        Validator::throwError(errorMsg, 124);
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
        std::string errorMsg = std::string(ZSTR_VAL(this->key)) + " NOT MATCH REGEX";
        Validator::throwError(errorMsg, 125);
        return false;
    }

    result = data;

    return true;
}

bool Validator::doCheck()
{
    std::shared_ptr<zval> typePtr = MyApiTool::getZvalByHashTable(Z_ARRVAL_P(law), "type", false);

    if (!typePtr)
    {
        return false;
    }

    std::string type = Z_STRVAL_P(typePtr.get());

    std::shared_ptr<zval> sourcePtr = MyApiTool::getZvalByHashTable(Z_ARRVAL_P(law), "source", false);

    if (!sourcePtr)
    {
        return false;
    }

    std::string source = Z_STRVAL_P(sourcePtr.get());

    std::shared_ptr<zval> dataPtr;

    if (this->key && source == "get")
    {
        dataPtr = MyApiTool::getZvalByHashTable(Z_ARRVAL_P(this->getData), this->key, false);
    }
    else if (this->key && source == "post")
    {
        dataPtr = MyApiTool::getZvalByHashTable(Z_ARRVAL_P(this->postData), this->key, false);
    }
    else if (this->key && source == "file" && this->fileData && Z_TYPE_P(this->fileData) == IS_ARRAY)
    {
        dataPtr = MyApiTool::getZvalByHashTable(Z_ARRVAL_P(this->fileData), this->key, false);
    }
    else if (this->numKey && source == "cli" && this->cliData && Z_TYPE_P(this->cliData) == IS_ARRAY)
    {
        dataPtr = MyApiTool::getZvalByHashTable(Z_ARRVAL_P(this->cliData), this->numKey, false);
    } else {
        Validator::throwError("data fetching error", 126);
        return false;
    }

    zval *data = dataPtr.get();

    std::shared_ptr<zval> requirePtr = MyApiTool::getZvalByHashTable(Z_ARRVAL_P(law), "require", false);

    if (requirePtr)
    {
        zval *requireVal = requirePtr.get();

        if (Z_TYPE_INFO_P(requireVal) == IS_TRUE && !dataPtr)
        {
            std::string errorMsg = std::string(ZSTR_VAL(this->key)) + " PARAM REQUIRED";

            Validator::throwError(errorMsg, 127);
            return false;
        }
    }

    bool ret;

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
        php_error_docref(NULL, E_WARNING, "TYPE NOT FOUND");
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
}

zval Validator::doValidator(zval *laws, zval *get_data, zval *post_data, zval *file_data, zval *cli_data, bool& isSuccess)
{
    zend_string *key;
    zend_long num_key;
    zval *val;
    Validator myApiValidator;
    zval result;
    array_init(&result);

    ZEND_HASH_FOREACH_KEY_VAL(Z_ARRVAL_P(laws), num_key, key, val)
        myApiValidator.setNumKey(num_key);
        myApiValidator.setKey(key);
        myApiValidator.setLaw(val);
        myApiValidator.setPostData(post_data);
        myApiValidator.setGetData(get_data);
        myApiValidator.setFileData(file_data);
        myApiValidator.setCliData(cli_data);

        bool ret = myApiValidator.doCheck();

        if (myApiValidator.getErrorCode() > 0)
        {
            isSuccess = false;
            zend_hash_clean(Z_ARRVAL(result));
            add_assoc_long(&result, "error_code", myApiValidator.getErrorCode());
            add_assoc_string(&result, "error_msg", myApiValidator.getErrorMsg().c_str());
            return result;
        }

        if (ret)
        {
            zval *ret = myApiValidator.getResult();

            if (key != NULL)
            {
                add_assoc_zval(&result, ZSTR_VAL(key), ret);
            }
            else
            {
                std::string k = "cli_" + std::to_string(num_key);
                add_assoc_zval(&result, k.c_str(), ret);
            }

        }
    ZEND_HASH_FOREACH_END();

    isSuccess = true;

    return result;
}
