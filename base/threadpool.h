// Use of this source code is governed by a BSD-style license
// that can be found in the License file.
//
// Author: Shuo Chen (chenshuo at chenshuo dot com)

#ifndef BASE_THREADPOOL_H
#define BASE_THREADPOOL_H

#include <deque>
#include <vector>
#include <mutex>
#include <functional>
#include <condition_variable>
#include <string>
#include <thread>

class ThreadPool
{
public:
	typedef std::function<void()> Task;

	explicit ThreadPool(const std::string& nameArg = std::string("ThreadPool"));
	~ThreadPool();

	// Must be called before start().
	void setMaxQueueSize(int maxSize) { maxQueueSize_ = maxSize; }
	void setThreadInitCallback(const Task& cb)
	{
		threadInitCallback_ = cb;
	}

	void start(int numThreads);
	void stop();

	const std::string& name() const
	{
		return name_;
	}

	size_t queueSize();

	// Could block if maxQueueSize > 0
	void run(Task f);

private:
	bool isFull()const;
	void runInThread();
 	Task take();

	std::mutex mutex_;

	std::condition_variable notEmpty_;
	std::condition_variable notFull_;

 	std::string name_;
	Task threadInitCallback_;
 	std::vector<std::unique_ptr<std::thread>> threads_;
 	std::deque<Task> queue_;
	size_t maxQueueSize_;
	bool running_;
};

#endif  // MUDUO_BASE_THREADPOOL_H
