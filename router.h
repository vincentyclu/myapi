#ifndef MYAPI_ROUTER_H

#define MYAPI_ROUTER_H

extern "C" {
    #include "php.h"
    #include "ext/standard/info.h"
    #include "php_myapi.h"
}

MYAPI_STARTUP_FUNCTION(router);

#endif // MYAPI_ROUTER_H

