#ifndef MYAPI_CONFIG_H

#define MYAPI_CONFIG_H

extern "C" {
    #include "php.h"
    #include "ext/standard/info.h"
    #include "php_myapi.h"
}

MYAPI_STARTUP_FUNCTION(config);

#endif // MYAPI_CONFIG_H

