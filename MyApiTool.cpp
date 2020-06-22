extern "C" {
    #ifdef HAVE_CONFIG_H
    # include "config.h"
    #endif

    #include "php.h"
    #include "ext/standard/info.h"
    #include "ext/standard/php_var.h"
    #include "ext/json/php_json.h"
    #include "zend_smart_str.h"
}
#include "php_myapi.h"
#include "MyApiTool.h"
#include "zend_exceptions.h"


MyApiTool::MyApiTool()
{
}

MyApiTool::~MyApiTool()
{
    if (Z_TYPE(this->object) == IS_OBJECT)
    {
        zval_ptr_dtor(&this->object);
    }
}

bool MyApiTool::isCallable()
{
    return callable_tag;
}

zval MyApiTool::getObject()
{
    return this->object;
}

MyApiTool::MyApiTool(std::string className)
{

    std::shared_ptr<zend_string> class_name_ptr = MyApiTool::getZendString(className.c_str());
    zend_class_entry *classEntry = zend_lookup_class(class_name_ptr.get());

    if (classEntry != NULL)
    {
        callable_tag = true;
        object_init_ex(&this->object, classEntry);
    }
}

std::shared_ptr<zval> MyApiTool::callMethod(const char *function_name, int params_count, zval params[])
{
    if (Z_TYPE(this->object) != IS_OBJECT)
    {
        return std::shared_ptr<zval>();
    }

    return MyApiTool::callMethodWithObject(this->object, function_name, params_count, params);
}

std::shared_ptr<zval> MyApiTool::callFunction(const char *function_name, int params_count, zval params[])
{
    zval* ret = (zval *) emalloc(sizeof(zval));

    std::shared_ptr<zval> ptr(ret, [](zval *p){
        zval_ptr_dtor(p);
        efree(p);
    });

    zval function_name_zval;

    std::shared_ptr<zval> functionPtr = MyApiTool::getZval(&function_name_zval, [&function_name_zval, &function_name](){
        ZVAL_STRING(&function_name_zval, function_name);
    });

    int call_result = call_user_function(NULL, NULL, functionPtr.get(), ret, params_count, params);

    if (FAILURE == call_result)
    {
        php_error_docref(NULL, E_WARNING, MYAPI_ERR_MSG(1), function_name);
        ptr.reset();
    }

    return ptr;
}

std::shared_ptr<zval> MyApiTool::callMethod(const char* class_name, const char *function_name, int params_count, zval params[])
{
    zval* ret = (zval *) emalloc(sizeof(zval));

    std::shared_ptr<zval> ptr(ret, [](zval *p){
        zval_ptr_dtor(p);
        efree(p);
    });

    zval function_name_zval;
    zval obj;

    std::shared_ptr<zval> objPtr = MyApiTool::getZval(&obj, [&obj, &class_name](){
        std::shared_ptr<zend_string> classNamePtr = MyApiTool::getZendString(class_name);

        object_init_ex(&obj, zend_lookup_class(classNamePtr.get()));
    });

    std::shared_ptr<zval> functionNamePtr = MyApiTool::getZval(&function_name_zval, [&function_name_zval, &function_name](){
        ZVAL_STRING(&function_name_zval, function_name);
    });

    int call_result = call_user_function(NULL, objPtr.get(), functionNamePtr.get(), ret, params_count, params);

    if (FAILURE == call_result)
    {
        php_error_docref(NULL, E_WARNING, MYAPI_ERR_MSG(1), function_name);
        ptr.reset();
    }

    return ptr;
}

std::shared_ptr<zval> MyApiTool::callMethodWithObject(zval& obj, const char *function_name, int params_count, zval params[])
{
    zval* ret = (zval *) emalloc(sizeof(zval));

    std::shared_ptr<zval> ptr(ret, [](zval *p){
        zval_ptr_dtor(p);
        efree(p);
    });

    zval function_name_zval;

    std::shared_ptr<zval> functionNamePtr = MyApiTool::getZval(&function_name_zval, [&function_name_zval, &function_name](){
        ZVAL_STRING(&function_name_zval, function_name);
    });

    int call_result = call_user_function(NULL, &obj, &function_name_zval, ret, params_count, params);

    if (FAILURE == call_result)
    {
        ptr.reset();
        php_error_docref(NULL, E_WARNING, MYAPI_ERR_MSG(1), function_name);
    }

    return ptr;
}

zval* MyApiTool::getData()
{
    zend_string *key_get = zend_string_init("_GET", strlen("_GET"), 0);

    zval *get_data = zend_hash_find(&EG(symbol_table), key_get);
    zend_string_release(key_get);

    return get_data;
}

zval MyApiTool::getLong(long num)
{
    zval val;
    ZVAL_LONG(&val, num);

    return val;
}

zval MyApiTool::getString(std::string str)
{
    zval val;
    ZVAL_STRING(&val, str.c_str());

    return val;
}

zval MyApiTool::getString(zend_string* str)
{
    zval val;
    ZVAL_STR(&val, str);

    return val;
}

/**
  * get zend string
  *
  */
std::shared_ptr<zend_string> MyApiTool::getZendString(const char* str, bool isDelete)
{
    zend_string *zstr = zend_string_init(str, strlen(str), 0);

    std::shared_ptr<zend_string> ptr(zstr, [isDelete](zend_string* zstr_ptr){
        if (isDelete)
        {
            zend_string_release(zstr_ptr);
        }
    });

    return ptr;
}

std::shared_ptr<zval> MyApiTool::getZval(zval *valPtr, std::function<void ()> initFun, bool isDelete)
{
    initFun();

    std::shared_ptr<zval> ptr(valPtr, [isDelete](zval *p){
        if (isDelete)
        {
            zval_ptr_dtor(p);
        }
    });

    return ptr;
}

/**
  * get zend constant
  *
  *
  */
std::shared_ptr<zval> MyApiTool::getContant(const char* str, bool isDelete)
{
    zval * constant = zend_get_constant(MyApiTool::getZendString(str).get());

    if (!constant)
    {
        return std::shared_ptr<zval>();
    }

    std::shared_ptr<zval> ptr(constant, [isDelete](zval *constantPtr){
        if (isDelete)
        {
            zval_ptr_dtor(constantPtr);
        }
    });

    return ptr;
}

std::shared_ptr<zval> MyApiTool::getContant(const char* str, zend_class_entry *entry, bool isDelete)
{
    zval * constant = zend_get_constant_ex(MyApiTool::getZendString(str).get(), entry, 0);

    if (!constant)
    {
        return std::shared_ptr<zval>();
    }

    std::shared_ptr<zval> ptr(constant, [isDelete](zval *constantPtr){
        if (isDelete)
        {
            zval_ptr_dtor(constantPtr);
        }
    });

    return ptr;
}

zval* MyApiTool::getZvalByHashTable(HashTable *ht, const char* str)
{
    zval *val = zend_hash_str_find(ht, str, strlen(str));

    return val;
}

zval* MyApiTool::getZvalByHashTable(HashTable *ht, zend_string* str)
{
    zval *val = zend_hash_find(ht, str);

    return val;
}

zval* MyApiTool::getZvalByHashTable(HashTable *ht, zend_long index)
{
    zval *val = zend_hash_index_find(ht, index);

    return val;
}

zval* MyApiTool::getZvalByHashTableEx(HashTable *ht, const char* key)
{
    char buf[100];
    strcpy(buf, key);
    char *k = strtok(buf, ".");
    HashTable *ht_c = ht;
    zval* retPtr;

    while (k != NULL)
    {
        if (ht_c == NULL)
        {
            retPtr = NULL;
            break;
        }

        retPtr = MyApiTool::getZvalByHashTable(ht_c, k);

        if (!retPtr)
        {
            break;
        }

        if (retPtr && Z_TYPE_P(retPtr) == IS_ARRAY)
        {
            ht_c = Z_ARRVAL_P(retPtr);
        }
        else
        {
            ht_c = NULL;
        }

        k = strtok(NULL, ".");
    }

    return retPtr;
}

void MyApiTool::throwException(std::string msg, zend_long code)
{
    std::shared_ptr<zend_string> classNamePtr = MyApiTool::getZendString("\\myapi\\Exception");
    const char* msgStr = msg.c_str();

    zend_throw_exception(zend_lookup_class(classNamePtr.get()), msgStr, code);
}

void MyApiTool::throwException(const char * msg, zend_long code)
{
    std::shared_ptr<zend_string> classNamePtr = MyApiTool::getZendString("\\myapi\\Exception");

    zend_throw_exception(zend_lookup_class(classNamePtr.get()), msg, code);
}
