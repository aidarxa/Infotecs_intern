#include <queue>
#include <condition_variable>
#include <mutex>
#include <optional>

class empty_queue : std::exception
{
    public:
    empty_queue(const std::string& message): message_(message){}
    const char* what() const noexcept override{
        return message_.c_str();
    }
    private:
    std::string message_;
};


// Версия потокобезопасной очереди с кучей блокировок....
template <class T>
class ThreadSafeQueue final{
    public:
    void push(const T& object){
        std::unique_lock<std::mutex> lock(mutex_);
        queue_.push(object);
        cond_.notify_one();
    }
    // Бросает исключения
    T pop(){
        std::unique_lock<std::mutex> lock(mutex_);
        if(queue_.empty()) throw empty_queue("Deleting item from empty queue.");
        T object = std::move(queue_.front());
        queue_.pop();
        return object;
    }
    // Не бросает исключения
    std::optional<T> try_pop(){
        std::unique_lock<std::mutex> lock(mutex_);
        if(queue_.empty()) return std::nullopt;
        T object = std::move(queue_.front());
        queue_.pop();
        return object;
    }
    std::optional<T> wait_and_pop(){
        std::unique_lock<std::mutex> lock(mutex_);
        cond_.wait(lock, [this](){return !queue_.empty() || stop_;});
        if(stop_ && queue_.empty()){
            return std::nullopt;
        }
        T object = std::move(queue_.front());
        queue_.pop();
        return object;
    }
    // Моментальный снимок
    bool empty(){
        std::unique_lock<std::mutex> lock(mutex_);
        return queue_.empty();
    };
    template <typename... Args>
    void emplace(Args&&... args){
        std::unique_lock<std::mutex> lock(mutex_);
        queue_.emplace(std::forward<Args>(args)...);
        cond_.notify_one();
    }
    void shutdown(){
        std::unique_lock<std::mutex> lock(mutex_);
        stop_ = true;
        cond_.notify_all();
    }

    ThreadSafeQueue() = default;
    
    //Копирование запрещено
    ThreadSafeQueue(const ThreadSafeQueue& cont) = delete;
    ThreadSafeQueue& operator=(const ThreadSafeQueue& other) = delete;

    //Перемещение запрещено
    ThreadSafeQueue(ThreadSafeQueue&& cont) = delete;
    ThreadSafeQueue& operator=(ThreadSafeQueue&& other) = delete;

    private:
    std::atomic<bool> stop_{false};
    std::queue<T> queue_;
    std::mutex mutex_;
    std::condition_variable cond_;
};
