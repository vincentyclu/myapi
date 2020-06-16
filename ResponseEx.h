#ifndef MYAPI_RESPONSE_EX_H
#define MYAPI_RESPONSE_EX_H

#include <string>
#include <memory>

class Response
{
public:
    std::string getErrorResult(std::string errorMsg, int errorCode);
    std::string getJsonString(zval *val);
    static Response getInstance();
};

#endif //MYAPI_RESPONSE_EX_H

