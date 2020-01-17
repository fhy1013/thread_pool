#include "test.h"
#include "thread_pool_advanced.h"

#include <chrono>
#include <iostream>
#include <mutex>

using namespace thread_pool_zone;

bool flag = false;
int count = 0;
std::mutex coutMtx;  // protect std::cout

const int g_num = 30;

void task(int taskId) {
	{
		std::lock_guard<std::mutex> guard(coutMtx);
		++count;
		// std::cout << "task-" << taskId << " begin! "
		// << std::this_thread::get_id() << std::endl;
	}
	// executing task for 2 second
	std::this_thread::sleep_for(std::chrono::milliseconds(50));

	{
		std::lock_guard<std::mutex> guard(coutMtx);
		// std::cout << "task-" << taskId << " end!" << std::endl;
	}
}

void monitor(const ThreadPool &pool) {
	while (!flag) {
		{
			std::lock_guard<std::mutex> guard(coutMtx);
			auto num = pool.CurrentThreads();
			auto idle = pool.IdleThreads();
			std::cout << "current threads : " << num << std::endl;
			std::cout << "idle threads : " << idle << std::endl;
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(1000));
	}
	std::cout << "monitor func end" << std::endl;
}

bool Default() {
	ThreadPool pool;

	// pool.commit(monitor, std::ref(pool));
	std::this_thread::sleep_for(std::chrono::milliseconds(100));
	if (1 != pool.CurrentThreads()) {
		return false;
	}
	if (1 != pool.IdleThreads()) {
		return false;
	}

	for (int taskId = 0; taskId < g_num; ++taskId) {
		std::this_thread::sleep_for(std::chrono::milliseconds(1));
		pool.commit(task, taskId);
	}

	if (4 != pool.CurrentThreads()) {
		return false;
	}
	if (0 != pool.IdleThreads()) {
		return false;
	}
	while (count) {
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
		if (count >= g_num) {
			std::this_thread::sleep_for(std::chrono::milliseconds(2100));
			if (1 != pool.CurrentThreads()) {
				return false;
			}
			if (1 != pool.IdleThreads()) {
				return false;
			}
			break;
		}
	}
	// std::cout << "main end..." << std::endl;
	//{
	// std::lock_guard<std::mutex> guard(coutMtx);
	// flag = true;
	//}
	return true;
}

bool FixedThreads() {
	ThreadPool pool(4);

	// pool.commit(monitor, std::ref(pool));
	std::this_thread::sleep_for(std::chrono::milliseconds(100));
	if (4 != pool.CurrentThreads()) {
		return false;
	}
	if (4 != pool.IdleThreads()) {
		return false;
	}

	for (int taskId = 0; taskId < g_num; ++taskId) {
		std::this_thread::sleep_for(std::chrono::milliseconds(1));
		pool.commit(task, taskId);
	}

	if (4 != pool.CurrentThreads()) {
		return false;
	}
	if (0 != pool.IdleThreads()) {
		return false;
	}
	while (count) {
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
		if (count >= g_num) {
			std::this_thread::sleep_for(std::chrono::milliseconds(2100));
			if (4 != pool.CurrentThreads()) {
				return false;
			}
			if (4 != pool.IdleThreads()) {
				return false;
			}
			break;
		}
	}
	return true;
}

bool AutomaticThreads() {
	ThreadPool pool(2, 4);

	// pool.commit(monitor, std::ref(pool));
	std::this_thread::sleep_for(std::chrono::milliseconds(100));
	if (2 != pool.CurrentThreads()) {
		return false;
	}
	if (2 != pool.IdleThreads()) {
		return false;
	}

	for (int taskId = 0; taskId < g_num; ++taskId) {
		std::this_thread::sleep_for(std::chrono::milliseconds(1));
		pool.commit(task, taskId);
	}

	if (4 != pool.CurrentThreads()) {
		return false;
	}
	if (0 != pool.IdleThreads()) {
		return false;
	}
	while (count) {
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
		if (count >= g_num) {
			std::this_thread::sleep_for(std::chrono::milliseconds(2100));
			if (2 != pool.CurrentThreads()) {
				return false;
			}
			if (2 != pool.IdleThreads()) {
				return false;
			}
			break;
		}
	}
	return true;
}

bool FixedToAutomatic() {
	ThreadPool pool(3);

	pool.SetThreadCount(2, 4);
	std::this_thread::sleep_for(std::chrono::milliseconds(2100));

	for (int taskId = 0; taskId < g_num; ++taskId) {
		std::this_thread::sleep_for(std::chrono::milliseconds(1));
		pool.commit(task, taskId);
	}
	if (4 != pool.CurrentThreads()) {
		return false;
	}
	if (0 != pool.IdleThreads()) {
		return false;
	}
	while (count) {
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
		if (count >= g_num) {
			std::this_thread::sleep_for(std::chrono::milliseconds(2100));
			if (2 != pool.CurrentThreads()) {
				return false;
			}
			if (2 != pool.IdleThreads()) {
				return false;
			}
			break;
		}
	}
	return true;
}

bool AutomaticToFixed(){
	ThreadPool pool(2, 4);

	pool.SetThreadCount(3);

	for (int taskId = 0; taskId < g_num; ++taskId) {
		std::this_thread::sleep_for(std::chrono::milliseconds(1));
		pool.commit(task, taskId);
	}

	if (3 != pool.CurrentThreads()) {
		return false;
	}
	if (0 != pool.IdleThreads()) {
		return false;
	}
	while (count) {
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
		if (count >= g_num) {
			std::this_thread::sleep_for(std::chrono::milliseconds(2100));
			if (3 != pool.CurrentThreads()) {
				return false;
			}
			if (3 != pool.IdleThreads()) {
				return false;
			}
			break;
		}
	}
	return true;
}
