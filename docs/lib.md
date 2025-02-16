# mylog: Простая библиотека логирования
## Описание
**mylog** – библиотека логирования на C++, поддерживающая пользовательские потоки вывода.

Библиотека поддерживает следующие уровни важности:  
${\small\textsf{\color{#B2FBA5}INFO}}$  
${\small\textsf{\color{#FFC067}WARNING}}$  
${\small\textsf{\color{#fe6761}ERROR}}$

Документация по тестовому приложению находится [здесь](application.md#приложение-для-тестирования-бибилиотеки).
## Установка
Для использования в своем проекте [соберите библиотеку](/Readme.md#быстрый-старт) и включите заголовочный файл 
```
#include "Logger.h" 
```  

### Связывание 
Выполните связываение библиотеки с вашим приложением

```make
OUT_DIR = out
INCLUDE = lib/include
OUT_FILENAME = a.out
CXXFLAGS = -std=c++17 -o OUT_FILENAME $(APPLICATION_SOURCES)/*.cpp -I$(INCLUDE)
g++ $(CXXFLAGS) -L$(OUT_DIR) -llogger -Wl,-rpath=$(PWD)/$(OUT_DIR)
```  

## Использование
### Вывод в файл
```cpp
#include <iostream>
#include "Logger.h"

int main(){
    mylog::FileOutput out{"test.log"};
    mylog::Logger logger{out,mylog::Level::INFO};
    try{
        logger.writeLog("Пробная запись с уровнем важности по умолчанию");
        logger.writeLog("Информация",mylog::Level::INFO);
        logger.writeLog("Предупреждение",mylog::Level::WARNING);
        logger.writeLog("Ошибка",mylog::Level::ERROR);
    }catch(mylog::logger_exception e){
        std::cerr << "Не удалось записать в лог" << e.what();
    }
}
```

### Вывод в пользовательский поток  
Вы можете определить свой поток вывода, для этого реализуйте интерфейс [```mylog::ILogOutput```](#ilogoutput). 

Например:
```cpp
#include <iostream>
#include "Logger.h"

class ConsoleOutput: public mylog::ILogOutput {
public:
    void write(std::string_view message) override {
        std::cout << message;
    }
};

int main() {
    ConsoleOutput out;
    mylog::Logger logger{out,mylog::Level::INFO };
    try {
        logger.writeLog("Пробная запись с уровнем важности по умолчанию");
        logger.writeLog("Информация", mylog::Level::INFO);
        logger.writeLog("Предупреждение", mylog::Level::WARNING);
        logger.writeLog("Ошибка", mylog::Level::ERROR);
    }
    catch (mylog::logger_exception e) {
        std::cerr << "Не удалось записать в лог" << e.what();
    }
}
```
## Описание классов и функций
Все классы и функции определены в пространстве имён ```mylog```, вспомогательные функции находятся в пространстве имён ```mylog::util```.
### ILogOutput
Интерфейс ```ILogOutput``` позволяет реализовать собственные механизмы вывода логов.
```cpp
class ILogOutput{
    public:
    virtual void write(std::string_view message) = 0;
    virtual ~ILogOutput()= default;
};
```
### FileOutput
```FileOutput``` реализует интерфейс ```ILogOutput``` и записывает сообщения в файл (```std::ofstream```).  
```cpp
class FileOutput:public ILogOutput{
    public:
    FileOutput(std::string_view filename);
    void write(std::string_view message) override;
    private:
    std::string filename_;
};
```
#### Примечание
Поток открывается с флагом ```std::ios_base::app``` и всегда дописывает сообщения в конец файла.
#### Ошибки
В случае неудачной попытки записи в файл функция ```write``` выбросит исключение [```logger_exception```](#logger_exception).
### logger_exception
Стандартное исключение, используется функцией ```write``` класса [```FileOutput```](#fileoutput)
```cpp
class logger_exception: public std::exception{
    public:
    logger_exception(std::string_view message);
    const char* what()const noexcept override;
    private:
    std::string message_;
};
```
### Level
Класс перечислений, определяет уровни важности сообщений.
```cpp
enum class Level{
    INFO,
    WARNING,
    ERROR
};
```
> [!IMPORTANT]  
> Если вы решите добавить или изменить уровни важности, вам следует также модифицировать соотсветсвующий фунционал в [```mylog::logLevelToString```](#logleveltostring) и [```mylog::stringToLogLevel```](#stringtologlevel)

### Logger
Базовый класс для записи сообщений в файл, требует реализации[```ILogOutput```](#ilogoutput), например [```FileOutput```](#fileoutput).
```cpp
class Logger {
    public:
    Logger(ILogOutput& output, Level defaultLevel);
    virtual ~Logger() = default;

    void setDefaultLogLevel(Level defaultLevel);
    virtual void writeLog(const std::string& message, Level level);
    void writeLog(const std::string& message);

    protected:
    ILogOutput& output_;
    Level defaultLevel_;
};
```
|Функция|Описание|
|---|---|
|setDefaultLogLevel|Устанавливает новое значение уровня важности по умолчанию|
|writeLog +1|Записывает данные в файл|
### setDefaultLogLevel()
Устанавливает новое значение уровня важности по умолчанию.
#### Синтаксис
```cpp
void setDefaultLogLevel(mylog::Level defaultLevel);
```
#### Параметры
[```mylog::Level```](#level)

#### Возвращаемое значение
Нет
### writeLog()
Пишет ```message``` с указанным ```level``` в объект ```ILogOutput```
#### Синтаксис
```cpp
virtual void writeLog(const std::string& message, mylog::Level level);
```
#### Параметры
```const std::string& message```,[```mylog::Level level```](#level)

#### Возвращаемое значение
Нет
> [!NOTE]  
> По умолчанию записывает сообщения в формате:
> 
> ```[дд.мм.гггг чч:мм:сс] Level: сообщение```
> 
> Пример:
> 
> ```[16.02.2025 13:27:54] WARNING: Пример```
### +writeLog()
**Перегрузка**, пишет ```message``` с уровнем важности по умолчанию в объект ```ILogOutput```
#### Синтаксис
```cpp
void writeLog(const std::string& message);
```
#### Параметры
```const std::string& message```

#### Возвращаемое значение
Нет
> [!NOTE]  
> По умолчанию записывает сообщения в формате:
> 
> ```[дд.мм.гггг чч:мм:сс] Level: сообщение```
> 
> Пример:
> 
> ```[16.02.2025 13:27:54] WARNING: Пример```
### Примечание
> [!TIP]  
> Вы можете изменить формат вывода, унаследовавшись и реализовав собственную логику ```writeLog(const std::string& message, Level level)``` и ```writeLog(const std::string& message)```
> 
> ```writeLog(const std::string& message)``` является прослойкой для вызова своей перегрузки с уровнем важности по умолчанию и реализована так:
> ```cpp
> void mylog::Logger::writeLog(const std::string& message)
> {
>     writeLog(message, defaultLevel_);
> }
> ```

## Вспомогательные функции
### getCurrentDateTime()
Возвращает дату и время в формате ```дд.мм.гггг чч:мм:сс```, например: 
```16.02.2025 13:13:22```.
#### Синтаксис
```cpp
std::string getCurrentDateTime();
```
#### Параметры
Нет
#### Возвращаемое значение
```std::string```
#### Примечание
Может быть не потокобезопасной см. [localtime](https://en.cppreference.com/w/c/chrono/localtime).
### logLevelToString()
Функция обратная [```stringToLogLevel()```](#stringtologlevel). Переводит уровень важности ```mylog::Level``` сообщения в строку в верхнем регистре.
#### Синтаксис
```cpp
std::string logLevelToString(mylog::Level level);
```
#### Параметры
[```mylog::Level level```](#level)
#### Возвращаемое значение
```std::string```
#### Примечание
Если перечисление [```Level```](#level) изменяется пользователем, функция требует модификации.
> [!TIP]  
> Добавление нового значения UNKNOWN в ```Level```:
> ```cpp
> enum class Level{
>     UNKNOWN,
>     INFO,
>     WARNING,
>     ERROR
> }; 
> ```
>
> Тогда функция ```logLevelToString``` может определяться так:
> ```cpp
> std::string mylog::util::logLevelToString(Level level)
> {
>     switch (level)
>     {
>     case Level::UNKNOWN:
>         return "UNKNOWN";  
>     case Level::INFO:
>         return "INFO";
>     case Level::WARNING:
>         return "WARNING";
>     case Level::ERROR:
>         return "ERROR";
>     default:
>         return "";
>     }
> }
> ```
### stringToLogLevel()
Функция обратная [```logLevelToString()```](#logleveltostring). Переводит строку в [```mylog::Level```](#level), возвращает ```std::nullopt``` если совпадаения не найдены. **Регистронезависима**.
#### Синтаксис
```cpp
std::optional<mylog::Level> stringToLogLevel(std::string_view string);
```
#### Параметры
```std::string_view string```
#### Возвращаемое значение
```std::optional<mylog::Level>```
> [!TIP]  
> Вы можете проверить существует ли объект выполнив ```if (optional) {...}```
> 
> Для доступа к объекту используйте ```operator*()``` или ```value()```.
#### Примечание
Если перечисление [```mylog::Level```](#level) изменяется пользователем, функция требует модификации.
> [!TIP]  
> Добавление нового значения UNKNOWN в ```Level```:
> ```cpp
> enum class Level{
>     UNKNOWN,
>     INFO,
>     WARNING,
>     ERROR
> }; 
> ```
>
> Тогда функция ```stringToLogLevel()``` может определяться так:
> ```cpp
> std::optional<mylog::Level> mylog::util::stringToLogLevel(std::string_view string)
> {
>     std::string temp{string};
>     std::transform(temp.begin(),temp.end(),temp.begin(),::toupper);
>     if(temp == "UNKNOWN") return Level::UNKNOWN;
>     else if(temp == "INFO") return Level::INFO;
>     else if(temp == "WARNING") return Level::WARNING;
>     else if(temp == "ERROR") return Level::ERROR;
>     else return std::nullopt;
> }
> ```

> [!NOTE]  
> Функция не чувствительна к регистру. Ввод ```iNfo```, ```info``` или ```INFO``` вернет ```Level::INFO```

