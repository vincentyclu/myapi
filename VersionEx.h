#ifndef MYAPI_VERSION_EX_H
#define MYAPI_VERSION_EX_H

#include <string>
#include <memory>

class Version
{
public:
    static Version getInstance();
    void setResult(zval *result);
    zval *getResult();
};

#endif //MYAPI_VERSION_EX_H


