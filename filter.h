#ifndef MYAPI_FILTER_H

#define MYAPI_FILTER_H

extern "C" {
    #include "php.h"
    #include "ext/standard/info.h"
    #include "php_myapi.h"
}

MYAPI_STARTUP_FUNCTION(filter);

#endif // MYAPI_FILTER_H


