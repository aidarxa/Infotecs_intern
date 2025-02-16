#pragma once

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
    // Исключения для FileOutput
    class logger_exception: public std::exception{
        public:
        logger_exception(std::string_view message);
        const char* what()const noexcept override;
        private:
        std::string message_;
    };
    // Список доступных уровеней важности
    enum class Level{
        INFO,
        WARNING,
        ERROR
    };
    // Logger - осуществляет запись пользовательских сообщений в поток вывода
    // Формат вывода: 
    // [дд.мм.гггг чч:мм:сс] Level: message
    //
    // Пример: 
    //
    // #include <iostream>
    // #include "Logger.h"
    //  int main(){
    //      mylog::FileOutput out{"test.log"};
    //      mylog::Logger logger{out,mylog::Level::INFO};
    //      try{
    //          logger.writeLog("Пробная запись с уровнем важности по умолчанию");
    //          logger.writeLog("Информация",mylog::Level::INFO);
    //      }catch(mylog::logger_exception e){
    //            std::cerr << "Не удалось записать в лог" << e.what();
    //      }
    //}

    class Logger {
        public:
    
        Logger(ILogOutput& output, Level defaultLevel);
        virtual ~Logger() = default;
    
        Logger(const Logger& cont) = default;
        Logger& operator=(const Logger& other) = default;
    
        Logger(Logger&& cont) = default;
        Logger& operator=(Logger&& other) = default;

        // Обновление уровня важности по умолчанию
        void setDefaultLogLevel(Level defaultLevel);

        // Запись в ILogOutput
        virtual void writeLog(const std::string& message, Level level);
        void writeLog(const std::string& message);
    
        protected:
        ILogOutput& output_;
        Level defaultLevel_;
    };
    // Вспомогательные функции
    namespace util{
        // Получить дату и время в формате дд.мм.гггг чч:мм:сс
        std::string getCurrentDateTime();

        std::string logLevelToString(Level level);
        std::optional<Level> stringToLogLevel(std::string_view string);
    }
}