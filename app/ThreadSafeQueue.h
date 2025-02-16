#pragma once

#include <queue>
#include <condition_variable>
#include <mutex>
#include <optional>

// Потокобезопасная очередь, почти стандартный интерфейс
template <class T>
class ThreadSafeQueue final{

    public:
    void push(const T& object);

    // pop() + front(), поскольку стандартный набор операций: queue.front() + queue.pop() небезопасен в многопотоке
    std::optional<T> try_pop();
    //То же самое что и try_pop(), но ждёт пока появится элемент в очереди. Требует вызова shutdown() для безопасного завершения работы
    std::optional<T> wait_and_pop(); 

    // Только моментальный снимок, состояние может измениться сразу после выхода, наверное стоит вообще убрать...
    bool empty() const;

    template <typename... Args>
    void emplace(Args&&... args);

    // Завершает работу очереди, устанавливая stop_ в true. Использование очереди после вызова shutdown() - UB
    void shutdown();

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

template <class T>
void ThreadSafeQueue<T>::push(const T &object)
{
    std::unique_lock<std::mutex> lock(mutex_);
    queue_.push(object);
    cond_.notify_one();
}

template <class T>
std::optional<T> ThreadSafeQueue<T>::try_pop()
{
    std::unique_lock<std::mutex> lock(mutex_);
    if(queue_.empty()) return std::nullopt;
    T object = std::move(queue_.front());
    queue_.pop();
    return object;
}

template <class T>
std::optional<T> ThreadSafeQueue<T>::wait_and_pop()
{
    std::unique_lock<std::mutex> lock(mutex_);
    cond_.wait(lock, [this](){return !queue_.empty() || stop_.load(std::memory_order_relaxed);});
    if(stop_.load(std::memory_order_relaxed) && queue_.empty()){
        return std::nullopt;
    }
    T object = std::move(queue_.front());
    queue_.pop();
    return object;
}

template <class T>
bool ThreadSafeQueue<T>::empty() const
{
    std::unique_lock<std::mutex> lock(mutex_);
    return queue_.empty();
}

template <class T>
void ThreadSafeQueue<T>::shutdown()
{
    stop_.store(true,std::memory_order_relaxed);
    cond_.notify_all();
}
template <class T>
template <typename... Args>
void ThreadSafeQueue<T>::emplace(Args &&...args)
{
    std::unique_lock<std::mutex> lock(mutex_);
    queue_.emplace(std::forward<Args>(args)...);
    cond_.notify_one();
}
