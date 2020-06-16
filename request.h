#ifndef MYAPI_REQUEST_H

#define MYAPI_REQUEST_H

extern "C" {
    #include "php.h"
    #include "ext/standard/info.h"
    #include "php_myapi.h"
}

MYAPI_STARTUP_FUNCTION(request);

#endif // MYAPI_REQUEST_H

