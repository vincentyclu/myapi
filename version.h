#ifndef MYAPI_VERSION_H

#define MYAPI_VERSION_H

extern "C" {
    #include "php.h"
    #include "ext/standard/info.h"
    #include "php_myapi.h"
}

MYAPI_STARTUP_FUNCTION(version);

#endif // MYAPI_VERSION_H

