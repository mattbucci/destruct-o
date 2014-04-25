#include "stdafx.h"
#include "SyncTask.h"

SyncTask::SyncTask() {
	asyncRunning = false;
	waitingOnTask = false;
}

//Indicate any calls to this will be handled by the polling thread
void SyncTask::PollingThreadStart() {
	asyncRunning = true;
}

//Poll and execute any waiting tass
void SyncTask::PollingThreadPoll() {
	lock_guard<mutex> protect(taskMutex);

	if (waitingOnTask) {
		waitingOnTask= false;
		asyncTask();
		taskWatch.notify_all();
	}
}

//runs the given task async if async is running
//or runs it sync if not
void SyncTask::RequestTask(function<void()> task) {
	if (asyncRunning) {
		unique_lock<mutex> check(taskMutex);

		asyncTask = task;
		waitingOnTask = true;
		taskWatch.wait(check);
		check.unlock();
	}
	else
		task();
}

//Indicate any calls to this will be handled on the current thread
void SyncTask::PollingThreadStop() {
	PollingThreadPoll();
	asyncRunning = false;
}