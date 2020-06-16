#ifndef MYAPI_LOG_EX_H
#define MYAPI_LOG_EX_H

#include <string>
#include <memory>
#include <map>

class Log
{
public:
    std::string getLogDir();
    static Log getInstance();
    bool writeLog(std::string msg, std::string level);
    bool isWriteLog(std::string level);

private:
    std::map<std::string, int> m{
        {"debug" , 1},{"info", 2},{"warning", 3},{"fatal", 4},
    };
};

#endif //MYAPI_LOG_EX_H

