
#ifndef CWORK_THREAD_H_
#define CWORK_THREAD_H_  

#include "CWorkerThread.h"
#include <iostream>
#include "macro.h"
#include "libLog.h"

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
		LOG_INFO("start a new round");
		CJob *job = this->getJob();
		LOG_INFO("will handle job %ld",(long int )job);
		if(NULL != job){
			job->Run();
			LOG_INFO("before delete job");
			SafeDelete(job);
			}
		LOG_INFO("handle a job!!");
	}
}

void CWorkerThread::addJob(CJob *job)
{
	std::lock_guard<std::mutex> lg(m_JobCondMutex);
	jobList.push_back(job);
	m_JobCond.notify_one();
}

CJob *CWorkerThread::getJob(void)
{
	TRACE_IN();
	std::unique_lock<std::mutex> lg(m_JobCondMutex);
	while(jobList.empty())
		m_JobCond.wait(lg);

	list<CJob *>::iterator  ite;
	if(jobList.size() > 0){
		LOG_INFO("jobLise size=%ld",jobList.size());
		ite = jobList.begin();
		jobList.pop_front();
		return *ite;
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
