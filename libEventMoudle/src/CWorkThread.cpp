
#ifndef CWORK_THREAD_H_
#define CWORK_THREAD_H_  

#include "CWorkerThread.h"
#include <iostream>

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
		if(NULL != job)
			job->Run();
		if(NULL != job){
			delete job;
			job = NULL;
		}
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
	std::unique_lock<std::mutex> lg(m_JobCondMutex);
	while(jobList.empty())
		m_JobCond.wait(lg);

	list<CJob *>::iterator  ite;
	if(jobList.size() > 0){
		ite = jobList.begin();
		jobList.pop_front();
		return *ite;
		}
	return NULL;
}
void CWorkerThread::SetThreadPool(CThreadPool *thrpool)
{
	std::lock_guard<std::mutex> lg(m_VarMutex);
	m_ThreadPool = thrpool;
}


#endif //CWORK_RHREAD_H_
