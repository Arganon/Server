#include "threadpool.h"
#include "logger.h"

ThreadPool::~ThreadPool() {
    stop();
}

void ThreadPool::add_task(Task _task) noexcept {
    {
        std::unique_lock<std::mutex> lock{m_mutex};
        m_tasks.emplace(std::move(_task));
    }
    m_cv.notify_one();
}

size_t ThreadPool::get_count_of_active_tasks() noexcept {
    return m_tasks.size();
}

void ThreadPool::set_thread_quantity(const int _amount) noexcept {
    m_is_stopping = false;
    if (_amount < 1) {
        LOG(LOG_LVL::LOGS::DEBUG, "Value of the thread amount is less than 1.");
        return;
    }
    start(_amount);
}

void ThreadPool::stop_all_threads() noexcept {
    stop();
}

void ThreadPool::start(const int _threadsCount) {
    for (int i = 0; i < _threadsCount; ++i) {
        m_threads.emplace_back
        (
        [=]{
                while (true) {
                    Task task;
                    {
                        std::unique_lock<std::mutex> lock{m_mutex};
                        m_cv.wait(lock, [=]{ return m_is_stopping || !m_tasks.empty(); });

                        if (m_is_stopping) {
                            break;
                        }
                        task = std::move(m_tasks.front());
                        m_tasks.pop();
                    }

                    task();
                }
            }
        );
    }
}

void ThreadPool::stop() {
    {
        std::unique_lock<std::mutex> lock{m_mutex};
        m_is_stopping = true;
    }

    m_cv.notify_all();
    for (auto& thread : m_threads) {
        thread.detach();
    }
    m_threads.clear();

    std::queue<Task> empty;
    std::swap(m_tasks, empty);
}
