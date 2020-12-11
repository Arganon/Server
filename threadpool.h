#ifndef THREADPOOL_H
#define THREADPOOL_H

#include <queue>
#include <mutex>
#include <vector>
#include <thread>
#include <condition_variable>

using Task = std::function<void()>;

/// @brief Classical type of the thread pool.
class ThreadPool {
    private:
        /// @brief The using threads vector
        std::vector<std::thread> m_threads;

        /// @brief Queue of the tasks
        std::queue<Task> m_tasks;

        /// @brief The value for threads call synchronization.
        std::condition_variable m_cv;

        /// @brief The value to block threads.
        std::mutex m_mutex;

        /// @brief The value to stop the thread pool.
        volatile bool m_is_stopping;

        /// @brief Execute the thread pool.
        /// @param[in] _threadsCount Total count of the working threads.
        void start(const int _threadsCount);

        /// @brief Stop executing of the thread pool.
        void stop();

    public:
        /// @brief Default Constructor.
        ThreadPool() = default;

        /// @brief Destructor.
        ~ThreadPool();

        /// @brief Add task to the queue for thread execution.
        /// @param[in] _task Callback function.
        void add_task(Task _task) noexcept;

        /// @brief Set total amount of the thread quantity and execute them.
        /// @param[in] _amount Total threads amount.
        void set_thread_quantity(const int _amount) noexcept;

        /// @brief Returns the total count of the active tasks.
        /// return Total count of the active tasks.
        size_t get_count_of_active_tasks() noexcept;

        /// @brief
        /// @param[in]
        void stop_all_threads() noexcept;
};

#endif // THREADPOOL_H
