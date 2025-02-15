#include "Logger.h"

mylog::Logger::Logger(ILogOutput& output,Level defaultLevel)
    :output_(output),
    defaultLevel_(defaultLevel)
{
}

void mylog::Logger::setDefaultLogLevel(Level defaultLevel)
{
    defaultLevel_ = defaultLevel;
}

std::string mylog::util::getCurrentDateTime()
{
    time_t now = time(NULL);
	struct tm* time = localtime(&now);
	char buf[20];
	strftime(buf, sizeof(buf), "%d.%m.%Y %H:%M:%S", time);
	return buf;
}
void mylog::Logger::writeLog(const std::string& message, Level level)
{
    if(level >= defaultLevel_){
        output_.write("[" + util::getCurrentDateTime() + "] " + util::logLevelToString(level) + ": " + message + "\n");
    }
}

void mylog::Logger::writeLog(const std::string& message)
{
    writeLog(message,defaultLevel_);
}

std::string mylog::util::logLevelToString(Level level)
{
    switch (level)
    {
    case Level::INFO:
        return "INFO";
    case Level::WARNING:
        return "WARNING";
    case Level::ERROR:
        return "ERROR";
    default:
        return "";
    }
}

std::optional<mylog::Level> mylog::util::stringToLogLevel(std::string_view string)
{
    std::string temp{string};
    std::transform(string.begin(),string.end(),temp.begin(),::toupper);
    if(temp == "INFO") return Level::INFO;
    else if(temp == "WARNING") return Level::WARNING;
    else if(temp == "ERROR") return Level::ERROR;
    else return std::nullopt;
}

mylog::FileOutput::FileOutput(std::string_view filename)
    :filename_(filename)
{
}

void mylog::FileOutput::write(std::string_view message)
{
    std::ofstream log(filename_,std::ios_base::app);
    if(!log) throw logger_exception("Couldn't open the file: " + filename_ + "\nError: " + strerror(errno));
    log << message;
    if(!log) throw logger_exception("Error writing to a file: " + filename_ + "\nError: " + strerror(errno));
}

mylog::logger_exception::logger_exception(std::string_view message)
    :message_(message)
{
}

const char *mylog::logger_exception::what() const noexcept
{
    return message_.c_str();
}
