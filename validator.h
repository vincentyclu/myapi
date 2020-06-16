#ifndef MYAPI_VALIDATOR_H

#define MYAPI_VALIDATOR_H

extern "C" {
    #include "php.h"
    #include "ext/standard/info.h"
    #include "php_myapi.h"
}

MYAPI_STARTUP_FUNCTION(validator);

#endif // MYAPI_VALIDATOR_H

