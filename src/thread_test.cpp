#include "thread_pool.h"
#include <string>
#include <vector>
#include <iostream>
#include <chrono>

std::string func1(int x){
	std::cout<< "---"<<__FUNCTION__ << " " << std::this_thread::get_id()<<"---"<< std::endl;
	std::this_thread::sleep_for(std::chrono::milliseconds(700));
	return std::to_string(x * x);
}

std::string func2(double d, std::string s){
	std::cout<< "---"<<__FUNCTION__ << " " << std::this_thread::get_id()<<"---"<< std::endl;
	std::this_thread::sleep_for(std::chrono::milliseconds(200));
	return s + std::to_string(d);
}

void func_test(){
	ThreadZone::ThreadPool pool;
	std::vector<std::future<std::string>> res;

	for(auto i = 0; i < 70; ++i){
		if(i%2 == 0)
			res.emplace_back(pool.commit(func1, i+1));
		else
			res.emplace_back(pool.commit(func2, i+1, std::to_string(i)));

		std::cout<<" thread count: "<<pool.threadCount() << " ***"<<std::endl;
		std::cout<<"idle thread count: " << pool.idlCount() << " ***" <<std::endl;
	}

	std::cout<< __FUNCTION__ << " commit all "<<std::endl;

	for(auto &&e : res)
		std::cout<<e.get() << ' ';
	std::cout<<std::endl;
}
