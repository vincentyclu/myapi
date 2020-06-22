#ifndef MYAPI_VALIDATOR_INTERNAL_H
#define MYAPI_VALIDATOR_INTERNAL_H

#include <vector>
#include <string>

extern "C" {
    #include "php.h"
}
#include <memory>

class Validator
{
public:
    Validator();

    bool checkInt(zval *data);
    bool checkDouble(zval *data);
    bool checkString(zval *data);
    bool checkDate(zval *data);
    bool checkFile(zval *data);
    bool checkRegex(zval *data);
    bool doCheck();

    void setKey(zend_string *key);
    void setNumKey(zend_long numKey);
    void setLaw(zval *law);
    void setGetData(zval *getData);
    void setPostData(zval *postData);
    void setFileData(zval *fileData);
    void setCliData(zval *cliData);
    zval *getResult();

    std::string getErrorMsg();
    int getErrorCode();
    void throwError(std::string msg, int code);

    static bool doValidator(zval *laws, zval *getData, zval *postData, zval *fileData, zval *cliData, zval *result);
private:
    zval *law = NULL;
    zval *getData = NULL;
    zval *postData = NULL;
    zval *fileData = NULL;
    zval *cliData = NULL;
    zval *result = NULL;
    zend_string *key = NULL;
    zend_long numKey = 0;
    std::string error_msg;
    int error_code = 0;
};

#endif // MYAPI_VALIDATOR_INTERNAL_H
