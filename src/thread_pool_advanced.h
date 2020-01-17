#ifndef _THREAD_POOL_H_
#define _THREAD_POOL_H_

#include <atomic>
#include <condition_variable>
#include <functional>
#include <future>
#include <queue>
#include <stdexcept>
#include <thread>
#include <unordered_map>

namespace thread_pool_zone {

//线程池,可以提交变参函数或Lamda表达式的匿名函数执行,可以获取执行返回值

class ThreadPool {
public:
	using Task = std::function<void()>;
	using UniqueLock = std::unique_lock<std::mutex>;
	using GuardLock = std::lock_guard<std::mutex>;
	using Thread = std::thread;
	using ThreadID = std::thread::id;

public:
	// 默认构造，最大线程数取硬件支持最大线程数
	ThreadPool() : ThreadPool(1, Thread::hardware_concurrency()) {}
	// 指定线程数不变
	ThreadPool(const size_t threads) : ThreadPool(threads, threads) {}
	// 指定线程最小和最大值，线程自动增减
	ThreadPool(const size_t min_threads, const size_t max_threads);

	ThreadPool(const ThreadPool &) = delete;
	ThreadPool &operator=(const ThreadPool &) = delete;

	~ThreadPool();

	// 提交一个任务
	template <class F, class... T>
	auto commit(F &&f, T &&... t)
		-> std::future<typename std::result_of<F(T...)>::type> {
		if (!run_)  // stop == true
			throw std::runtime_error("commit on ThreadPool is stopped.");

		using RetType = typename std::result_of<F(T...)>::type;
		auto task = std::make_shared<std::packaged_task<RetType()>>(
			std::bind(std::forward<F>(f), std::forward<T>(t)...));

		auto result = task->get_future();

		{
			// 添加任务到队列
			GuardLock lock(mtx_);
			tasks_.emplace([task]() { (*task)(); });
		}
		if (idle_threads_ < 1 && current_threads_ < max_threads_)
			IncreaseThread(1);

		cv_.notify_one();  // 唤醒一个线程执行

		return result;
	}

	//空闲线程数
	size_t IdleThreads() const {
		GuardLock lck(mtx_);
		return idle_threads_;
	}

	//当前线程数
	size_t CurrentThreads() const {
		GuardLock lck(mtx_);
		return current_threads_;
	}

	// 设置线程数量(固定)
	void SetThreadCount(const size_t threads) {
		SetThreadCount(threads, threads);
	}

	// 设置线程数量(线程数在最小与最大之间自动变化)
	void SetThreadCount(const size_t min_threads, const size_t max_threads) {
		GuardLock lck(mtx_);
		if (min_threads > max_threads) {
			min_threads_ = max_threads;
			max_threads_ = min_threads;
		} else {
			min_threads_ = min_threads;
			max_threads_ = max_threads;
		}
	}

private:
	// 添加指定数量的线程
	void IncreaseThread(size_t size);

	// 工作函数
	void Work();

	// 结束 结束线程队列 中的线程
	void FinishedThreads();

private:
	// 线程池是否关执行
	std::atomic<bool> run_;

	// 线程池
	// std::vector<std::thread> pool_;
	std::queue<ThreadID> finished_threads_;
	std::unordered_map<ThreadID, Thread> threads_;
	// 任务队列
	std::queue<Task> tasks_;
	// 同步
	mutable std::mutex mtx_;
	// 条件阻塞
	std::condition_variable cv_;

	// 当前线程数
	size_t current_threads_;
	// 空闲线程数
	size_t idle_threads_;
	// 最小线程数
	size_t min_threads_;
	// 最大线程数
	size_t max_threads_;
	// 阻塞等待超时时间(s)
	size_t wait_time_;
};

}  // namespace thread_pool_zone

#endif  // _THREAD_POOL_H_
