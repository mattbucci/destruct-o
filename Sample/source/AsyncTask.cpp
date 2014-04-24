#include "stdafx.h"
#include "AsyncTask.h"
#include <thread>

AsyncTask::AsyncTask(function<void()> task) {
	finished = false;
	started = false;
	this->task = task;
}

//Start the task (asynchronously)
void AsyncTask::Start() {
	if (started)
		return;

	thread thr([this]() {
		task();
		finished = true;
	});
	thr.detach();

	started = true;
}

//Check if it finished
bool AsyncTask::IsDone() {
	return finished;
}