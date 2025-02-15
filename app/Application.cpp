#include "Logger.h"
#include <iostream>
#include <thread>
#include <atomic>
#include "ThreadSafeQueue.h"

ThreadSafeQueue<std::function<void()>> queue;

void run(){
    while (true) {
        auto task = queue.wait_and_pop();
        if(!task){
            break;
        }
        try{
            (*task)();
        }catch(const mylog::logger_exception& e){
            std::cerr << e.what();
        }
    }
}

//Параметры - имя файла журнала и уровень важности по умолчанию
int main(int argc, char* argv[]){

    // Проверки ввода
    if(argc != 3){
        std::cout << "Неверное количество аргументов\n";
        return -1;
    }

    std::string str = argv[2];
    if(!mylog::util::stringToLogLevel(str)){
        std::cout << "Строка уровня важности не была распознана\n";
        return -1;
    }

    //Основная часть
    mylog::FileOutput out{argv[1]};
    mylog::Logger logger{out,*mylog::util::stringToLogLevel(str)};
    std::thread t(run);
    std::cout << "Вводите уровень важности и сообщение в формате: <level> <message>\n";
    
    // quit чтобы завершить работу
    while (true)
    {
        std::string input,inputLevel;
        getline(std::cin,input);
        if(input == "quit") break;
        auto pos = input.find_first_of(" ");
        inputLevel = input.substr(0,pos);
        if(auto level = mylog::util::stringToLogLevel(inputLevel)) {
            queue.push([message = input.substr(pos+1,input.length()),level = *level,logger]()mutable{logger.writeLog(message,level);});
        }
        else{
            queue.push([message = input,logger]()mutable{logger.writeLog(message);});
        }
    }
    queue.shutdown();
    t.join();
    return 0;
}