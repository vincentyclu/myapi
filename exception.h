#ifndef MYAPI_EXCEPTION_H

#define MYAPI_EXCEPTION_H

extern "C" {
    #include "php.h"
    #include "ext/standard/info.h"
    #include "php_myapi.h"
}

MYAPI_STARTUP_FUNCTION(exception);

#endif // MYAPI_EXCEPTION_H


