#include "thread_pool_advanced.h"

using namespace thread_pool_zone;

ThreadPool::ThreadPool(const size_t min_threads, const size_t max_threads)
	: run_(true),
	  current_threads_(0),
	  idle_threads_(0),
	  min_threads_(min_threads),
	  max_threads_(max_threads),
	  wait_time_(1) {
	if (min_threads > max_threads) {
		max_threads_ = min_threads;
		min_threads_ = max_threads;
	}
	//初始化线程数量
	IncreaseThread(min_threads_);
}

ThreadPool::~ThreadPool() {
	{
		GuardLock lck(mtx_);
		run_ = false;
	}
	cv_.notify_all();  // 唤醒所有线程线程
	for (auto &e : threads_) {
		if (e.second.joinable()) {
			e.second.join();  // 等待任务结束，线程会执行完成
		}
	}
}

void ThreadPool::IncreaseThread(size_t size) {
	for (; current_threads_ < max_threads_ && size > 0; --size) {
		Thread t(&ThreadPool::Work, this);
		if (threads_.find(t.get_id()) == threads_.end()) {
			threads_[t.get_id()] = std::move(t);
			++current_threads_;
		}
	}
}

void ThreadPool::Work() {
	while (true) {
		Task task;
		{
			UniqueLock lck(mtx_);
			++idle_threads_;
			auto timeout = !cv_.wait_for(lck, std::chrono::seconds(wait_time_), [this]() { return !run_ || !tasks_.empty(); });
			--idle_threads_;
			if (tasks_.empty()) {
				if (!run_) {
					--current_threads_;
					return;
				}
				if (timeout) {
					if (min_threads_ == max_threads_) continue;
					else {
						if (min_threads_ < current_threads_) {
							--current_threads_;
							FinishedThreads();
							finished_threads_.emplace(std::this_thread::get_id());
							return;
						} else {
							continue;
						}
					}
				}
			}
			task = std::move(tasks_.front());
			tasks_.pop();
		}
		task();
	}
}

void ThreadPool::FinishedThreads() {
	while (!finished_threads_.empty()) {
		auto id = std::move(finished_threads_.front());
		finished_threads_.pop();
		auto it = threads_.find(id);

		if (it != threads_.end() && it->second.joinable()) {
			it->second.join();
			threads_.erase(it);
		}
	}
}

