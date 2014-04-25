
#pragma once

#include "stdafx.h"
#include <mutex>
#include <condition_variable>

class SyncTask {
	bool asyncRunning;
	bool waitingOnTask;
	function<void()> asyncTask;

	mutex taskMutex;
	condition_variable taskWatch;
public:
	SyncTask();

	//Indicate any calls to this will be handled by the polling thread
	void PollingThreadStart();

	//Poll and execute any waiting tasks
	void PollingThreadPoll();

	//runs the given task async if async is running
	//or runs it sync if not
	void RequestTask(function<void()> task);

	//Indicate any calls to this will be handled on the current thread
	void PollingThreadStop();
};