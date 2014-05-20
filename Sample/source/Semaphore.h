
#pragma once

#include "stdafx.h"
#include <thread>
#include <mutex>
#include <condition_variable>

class semaphore
{
private:
	mutex mutex_;
	condition_variable condition_;
	long count_;

public:
	semaphore()
		: count_(0)
	{}

	void increase()
	{
		unique_lock<mutex> lock(mutex_);
		++count_;
		condition_.notify_one();
	}

	void decrease()
	{
		unique_lock<mutex> lock(mutex_);
		while(count_ <= 0)
			condition_.wait(lock);
		--count_;
	}
};