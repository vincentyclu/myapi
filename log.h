#ifndef MYAPI_LOG_H

#define MYAPI_LOG_H

extern "C" {
    #include "php.h"
    #include "ext/standard/info.h"
    #include "php_myapi.h"
}

MYAPI_STARTUP_FUNCTION(log);

#endif // MYAPI_LOG_H


