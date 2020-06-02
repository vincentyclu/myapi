#ifndef MYAPI_CONTROLLER_H

#define MYAPI_CONTROLLER_H

extern "C" {
    #include "php.h"
    #include "ext/standard/info.h"
    #include "php_myapi.h"
}

MYAPI_STARTUP_FUNCTION(controller);

#endif // MYAPI_CONTROLLER_H
