#ifndef MYAPI_ERROR_CODE_H
#define MYAPI_ERROR_CODE_H

#define MYAPI_ERR_MSG(error_code) ERROR_CODE_## error_code
#define MYAPI_ERR(error_code) MYAPI_ERR_MSG(error_code), error_code

#define MYAPI_ERR_RESPONSE(code, ...) {                                 \
     std::string msg = Config::getInstance().getErrorMsg(code);         \
     std::string error_msg = msg == "" ? MYAPI_ERR_MSG(code) : msg;     \
     char buf[100] = {};                                                \
     sprintf(buf, error_msg.c_str(), ##__VA_ARGS__);                    \
     Response::getInstance().setErrorResult(buf, code);                 \
}

#define ERROR_CODE_1 "function '%s' not existing"

//api and router
#define ERROR_CODE_100 "The parameter shoud be a directory"
#define ERROR_CODE_101 "FILE '%s' NOT EXISTING"
#define ERROR_CODE_102 "Api cannot be instantiated twice"
#define ERROR_CODE_103 "Filter of beginning error"
#define ERROR_CODE_104 "Action not found"
#define ERROR_CODE_105 "Controller not found"
#define ERROR_CODE_106 "Controller cannot be callable"
#define ERROR_CODE_107 "Filter of after error"
#define ERROR_CODE_150 "Router error"
#define ERROR_CODE_151 "Module cannot be empty"
#define ERROR_CODE_152 "Action cannot be empty"
#define ERROR_CODE_153 "Module not found"

//controller and validator
#define ERROR_CODE_200 "The law of validator should be an array"
#define ERROR_CODE_201 "The field '%s' not an integer"
#define ERROR_CODE_202 "The law of '%s' INVALID"
#define ERROR_CODE_203 "The value of '%s' is smaller than the min value"
#define ERROR_CODE_204 "The value of '%s' is bigger than the max value"
#define ERROR_CODE_205 "The field '%s' not a float"
#define ERROR_CODE_206 "The field '%s' not a string"
#define ERROR_CODE_207 "The length of field '%s' is smaller than the min length"
#define ERROR_CODE_208 "The length of field '%s' is bigger than the max length"
#define ERROR_CODE_209 "The field '%s' not date"
#define ERROR_CODE_210 "The field '%s' not file"
#define ERROR_CODE_211 "The size of field '%s' is bigger than the specified value"
#define ERROR_CODE_212 "The type of field '%s' is invalid"
#define ERROR_CODE_213 "The field '%s' is not regex"
#define ERROR_CODE_214 "The field '%s' cannot match the regex"
#define ERROR_CODE_215 "The field '%s' cannot fetch datas"
#define ERROR_CODE_216 "The field '%s' required"
#define ERROR_CODE_217 "The type of field '%s' not found"
#define ERROR_CODE_218 "The field must be a string"


//filter and version
#define ERROR_CODE_301 "Filter '%s' NOT FOUND"

//log and config
#define ERROR_CODE_401 ""

//model and orm
#define ERROR_CODE_501 ""

//request and response
#define ERROR_CODE_601

#endif // MYAPI_ERROR_CODE_H
