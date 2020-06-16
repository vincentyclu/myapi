#ifndef MYAPI_FILTER_EX_H
#define MYAPI_FILTER_EX_H

#include <string>
#include <vector>
#include <memory>

class Filter
{
public:
    bool isDoFilter(zval *include_rule, zval *exclude_rule);
    std::vector<std::string> getFilter();
    bool doBeginFilter();
    bool doAfterFilter(zval *actionResult);
    static Filter getInstance();
};

#endif //MYAPI_FILTER_EX_H

