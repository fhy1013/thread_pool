#include "thread_pool.h"
#include <string>
#include <vector>
#include <iostream>
#include <chrono>

#include "log.h"

std::string func1(int x) {
	//	std::cout << "---" << __FUNCTION__ << " " << std::this_thread::get_id()
	//	<< "---" << std::endl;
	Log(LogLevel::Info, std::this_thread::get_id(), "start... ", x);
	std::this_thread::sleep_for(std::chrono::milliseconds(50));
	Log(LogLevel::Info, std::this_thread::get_id(), "stop");
	if (x == 70) std::cout << "function completed" << std::endl;
	return std::to_string(x * x) + " func1";
}

std::string func2(double d, std::string s) {
	//	std::cout << "---" << __FUNCTION__ << " " << std::this_thread::get_id()
	//	<< "---" << std::endl;
	Log(LogLevel::Info, std::this_thread::get_id(), "start... ", d, " ", s);
	std::this_thread::sleep_for(std::chrono::milliseconds(30));
	Log(LogLevel::Info, std::this_thread::get_id(), "stop...");
	if (!s.compare("69")) std::cout << "function completed" << std::endl;
	return s + std::to_string(d) + " func2";
}

void func_test() {
	ThreadZone::ThreadPool pool;
	// std::vector<std::future<std::string>> res;

	Log(LogLevel::Info, std::this_thread::get_id(), "begin...");
	for (auto i = 0; i < 70; ++i) {
		if (i % 2 == 0)
			pool.commit(func1, i + 1);
		else
			pool.commit(func2, i + 1, std::to_string(i));

		// std::cout<<" thread count: "<<pool.threadCount() << " "<<std::endl;
		// std::cout<<"idle thread count: " << pool.idlCount() << " "
		// <<std::endl;
		Log(LogLevel::Info, " thread count: ", pool.threadCount(), " i: ", i,
			" ");
		Log(LogLevel::Info, "idle thread count: ", pool.idlCount(), " i: ", i,
			" ");
	}

	Log(LogLevel::Info, "commit all ");
	std::cout<< __FUNCTION__ << " commit all "<<std::endl;
	Log(LogLevel::Info, "thread count: ", pool.threadCount());
	pool.setThreadCount(8);

	// std::cout << std::endl;
	Log(LogLevel::Info, std::this_thread::get_id(), "end...");
	getchar();
	Log(LogLevel::Info, "thread count: ", pool.threadCount());
}
