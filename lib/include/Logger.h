#include <fstream>
#include <chrono>
#include <optional>
#include <algorithm>
#include <string_view>
#include <cstring>

namespace mylog{

    //Интерфейс вывода чтобы пользователь мог сам добавить вывод в какие ему хочется места
    class ILogOutput{
        public:
        virtual void write(std::string_view message) = 0;
        virtual ~ILogOutput()= default;
    };
    // Реализация ILogOutput для ofstream
    class FileOutput:public ILogOutput{
        public:
        FileOutput(std::string_view filename);
        void write(std::string_view message) override;
        
        private:
        std::string filename_;
    };
    class logger_exception:std::exception{
        public:
        logger_exception(std::string_view message);
        const char* what()const noexcept override;
        private:
        std::string message_;
    };

    enum class Level{
        INFO,
        WARNING,
        ERROR
    };

    class Logger final{
        public:

        Logger(ILogOutput& output,Level defaultLevel);
        ~Logger() = default;

        Logger(const Logger& cont) = default;
        Logger& operator=(const Logger& other) = default;

        Logger(Logger&& cont) = default;
        Logger& operator=(Logger&& other) = default;

        void setDefaultLogLevel(Level defaultLevel);
        void writeLog(const std::string& message, Level level);
        void writeLog(const std::string& message);

        private:
        ILogOutput& output_;
        Level defaultLevel_;
    };

    namespace util{
        std::string getCurrentDateTime();
        std::string logLevelToString(Level level);
        std::optional<Level> stringToLogLevel(std::string_view string);
    }
}