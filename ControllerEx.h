#ifndef MYAPI_CONTROLLER_EX_H
#define MYAPI_CONTROLLER_EX_H

#include <string>
#include <memory>

class Controller
{
public:
    static Controller getInstance();
    zval doCheck(zval *obj, zval *laws, zval *get_data, zval *post_data, zval *file_data, zval *cli_data, bool& isSuccess);
};

#endif //MYAPI_CONTROLLER_EX_H

