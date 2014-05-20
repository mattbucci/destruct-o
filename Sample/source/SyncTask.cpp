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

		//Mark async not running so that any other requests will be handled on the same thread
		//note: because of this step this setup can only ever work with two threads, the polling thread
		//and one other thread. This could be fixed with a recursive mutex
		asyncRunning = false;

		asyncTask = task;
		waitingOnTask = true;
		taskWatch.wait(check);

		//Mark async running again
		asyncRunning = true;
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