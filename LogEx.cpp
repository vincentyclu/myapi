extern "C" {
    #ifdef HAVE_CONFIG_H
    # include "config.h"
    #endif

    #include "php.h"
    #include "ext/standard/info.h"
    #include "ext/standard/php_var.h"
    #include "ext/standard/file.h"
    #include "ext/date/php_date.h"
}
#include "php_myapi.h"
#include "MyApiTool.h"
#include "ApiEx.h"
#include "LogEx.h"
#include <memory>
#include <string.h>
#include <fstream>
#include <ConfigEx.h>

Log Log::getInstance()
{
    static Log log;

    return log;
}

std::string Log::getLogDir()
{
    std::string apiPath = Api::getInstance().getApiPath();

    return apiPath + "/log";
}

bool Log::isWriteLog(std::string level)
{
    std::shared_ptr<zval> sysPtr = Config::getInstance().get("sys", false);

    if (!sysPtr || Z_TYPE_P(sysPtr.get()) != IS_ARRAY)
    {
        return false;
    }

    zval* logPtr = MyApiTool::getZvalByHashTable(Z_ARRVAL_P(sysPtr.get()), "log_type");

    if (!logPtr || Z_TYPE(*logPtr) != IS_STRING)
    {
        return false;
    }

    std::string log_level = Z_STRVAL_P(logPtr);

    return this->m[level] >= this->m[log_level];
}

bool Log::writeLog(std::string msg, std::string level)
{
    if (!isWriteLog(level))
    {
        return false;
    }

    time_t now = time(NULL);
    tm * t = gmtime(&now);
    std::string logPath = getLogDir();
    std::string year = std::to_string(t->tm_year + 1900);
    std::string month = std::to_string(t->tm_mon + 1);
    std::string day = std::to_string(t->tm_mday);

    std::string fileDir = logPath + "/" + year + "/" + month;

    zval ret;
    php_stat(fileDir.c_str(), fileDir.length(), FS_IS_DIR, &ret);

    if (Z_TYPE(ret) == IS_FALSE)
    {
        bool isSuccess = php_stream_mkdir(fileDir.c_str(), 0744, PHP_STREAM_MKDIR_RECURSIVE | REPORT_ERRORS, NULL);

        if (!isSuccess)
        {
            return false;
        }
    }

    std::string fileName = fileDir + "/" + year + "-" + month + "-" + day;

    std::ofstream ofs(fileName, std::ios::app);

    if (!ofs.is_open())
    {
        return false;
    }

    std::string zeroStr = "0";
    std::string timeStr =  (t->tm_hour > 9 ? std::to_string(t->tm_hour) : zeroStr + std::to_string(t->tm_hour)) + ":" +
                           (t->tm_min > 9 ? std::to_string(t->tm_min) : zeroStr + std::to_string(t->tm_min)) + ":" +
                           (t->tm_sec > 9 ? std::to_string(t->tm_sec) : zeroStr + std::to_string(t->tm_sec));

    std::string recordMsg = timeStr + "[" + level + ")" + "]" + msg + "\n";

    ofs << recordMsg;

    ofs.close();

    return true;
}
