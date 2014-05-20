
#pragma once

#include "stdafx.h"
#include <thread>

class AsyncTask {
	bool finished;
	bool started;
	function<void()> task;
public:
	AsyncTask(function<void()> task);

	//Start the task (asynchronously)
	void Start();

	//Check if it finished
	bool IsDone();

	//Check if the task has been started
	bool IsStarted();

};