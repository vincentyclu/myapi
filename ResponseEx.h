#ifndef MYAPI_RESPONSE_EX_H
#define MYAPI_RESPONSE_EX_H

#include <string>
#include <memory>

class Response
{
public:
    bool isError();
    void header(char* line, bool rep = true, zend_long code = 200);
    void setErrorResult(std::string errorMsg, int errorcode);
    void setErrorResult(zval *errorMsg, int errorcode);
    std::string getJsonString(zval *val);
    std::string getXmlString(zval *val);
    std::string output();
    zval* getResult();
    void setResult(zval *val);
    static Response getInstance();

private:
    std::string getXml(zval *val);
};

#endif //MYAPI_RESPONSE_EX_H

