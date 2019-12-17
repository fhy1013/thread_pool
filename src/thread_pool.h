#ifndef __THREAD_POOL__
#define __THREAD_POOL__

#include <vector>
#include <queue>
#include <thread>
#include <atomic>
#include <condition_variable>
#include <future>
#include <functional>
#include <stdexcept>

namespace ThreadZone{
const int max_thread_num = 16;

//线程池,可以提交变参函数或Lamda表达式的匿名函数执行,可以获取执行返回值
//不支持类成员函数, 支持类静态成员函数或全局函数,Opteron()函数等

class ThreadPool{
public:
	ThreadPool(unsigned size = 4) :_run(true), _idle_thread(0){
		//初始化线程数量
		addThread(size);
	}
	~ThreadPool(){
		_run = false;
		_task_cv.notify_all(); // 唤醒所有线程线程
		for(std::thread &t : _pool){
			if(t.joinable()){
				t.join(); // 等待任务结束，线程会执行完成
			}
		}
	}

	// 提交一个任务
	// 调用.get()获取返回值会等待任务执行完,获取返回值
	// 有两种方法可以实现调用类成员，
	// 一种是使用   bind： .commit(std::bind(&Dog::sayHello, &dog));
	// 一种是用 mem_fn： .commit(std::mem_fn(&Dog::sayHello), &dog)
	template<class F, class... Args>
		auto commit(F&& f, Args&&... args) ->std::future<decltype(f(args...))>{
			if (!_run) // stop == true ??
				throw std::runtime_error("commit on ThreadPool is stopped.");

			using RetType = decltype(f(args...)); // typename std::result_of<F(Args...)>::type, 函数 f 的返回值类型
			auto task = std::make_shared<std::packaged_task<RetType()>>(
				std::bind(std::forward<F>(f), std::forward<Args>(args)...)
			); // 把函数入口及参数，打包（绑定）
			std::future<RetType> future = task->get_future();
			{
				// 添加任务到队列
				//对当前块的语句加锁  lock_guard 是 mutex 的 stack 封装类
				//构造的时候 lock()，析构的时候 unlock()
				std::lock_guard<std::mutex> lock{ _lock  };
				_tasks.emplace([task](){ // push(Task{...})
					(*task)();
				});
			}
			if(_idle_thread < 1 && _pool.size() < max_thread_num)
				addThread(1);

			_task_cv.notify_one(); // 唤醒一个线程执行

			return future;
		}

	//空闲线程数量
	int idlCount() { return _idle_thread; }

	// 线程数量
	int threadCount() { return _pool.size(); }

private:
	
	// 添加指定数量得线程
	void addThread(unsigned size){
		for(; _pool.size() < max_thread_num && size > 0; --size){
			_pool.emplace_back([this]{
				while(_run){
					Task task; // 获取一个待执行的task
					{
						std::unique_lock<std::mutex> lck(_lock);
						_task_cv.wait(lck, [this]{
							return !_run || !_tasks.empty();
						}); // wait 直到有 task
						if(!_run && _tasks.empty())
							return;
						task = move(_tasks.front()); // 去除队列头的task
						_tasks.pop();
					}
					_idle_thread--;
					task(); // 执行任务
					_idle_thread++;
				}
			});
			_idle_thread++;
		}
	}
private:
	using Task = std::function<void()>;
	// 线程池
	std::vector<std::thread> _pool;
	// 任务队列
	std::queue<Task> _tasks;
	// 同步
	std::mutex _lock;
	// 条件阻塞
	std::condition_variable _task_cv;
	// 线程池是否关执行
	std::atomic<bool> _run;
	// 空闲线程数量
	std::atomic<int> _idle_thread;
};

}

#endif // __THREAD_POOL___
