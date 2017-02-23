
#ifndef CWORK_THREAD_H_
#define CWORK_THREAD_H_  

#include "CWorkerThread.h"
#include <iostream>
#include "macro.h"
#include "libLog.h"
#include "pthread.h"

using namespace std;
CWorkerThread::CWorkerThread()
{
	m_ThreadPool = NULL;
}

CWorkerThread::~CWorkerThread()
{
}

void CWorkerThread::Run()
{
	for(; ;){
		CJob *job = this->getJob();
		if(NULL != job){
			job->Run();
			SafeDelete(job);
			}
	}
}

void CWorkerThread::addJob(CJob *job)
{
	std::unique_lock<std::mutex> lg(m_JobCondMutex);
	jobList.push_back(job);
	m_JobCond.notify_one();
}

CJob *CWorkerThread::getJob(void)
{
	TRACE_IN();
	std::unique_lock<std::mutex> lg(m_JobCondMutex);
	while(jobList.empty())
		m_JobCond.wait(lg);

	if(jobList.size() > 0){
		CJob *job = *(jobList.begin());
		jobList.pop_front();          //this will destroy the iterator who point to jobList.front();
		LOG_INFO("thread(id = %ld) has %ld jobs",pthread_self(),jobList.size());
		return job;
		}
	TRACE_OUT();
	return NULL;
}
void CWorkerThread::SetThreadPool(CThreadPool *thrpool)
{
	std::lock_guard<std::mutex> lg(m_VarMutex);
	m_ThreadPool = thrpool;
}


#endif //CWORK_RHREAD_H_
