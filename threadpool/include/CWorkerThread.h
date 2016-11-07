#ifndef CWORKTHREAD_H_
#define CWORKTHREAD_H_

#include <condition_variable>
#include <mutex>
#include "CThreadPool.h"
#include "CJob.h"

class CThreadPool;

class CWorkerThread:public CThread
{
private:
	CThreadPool *m_ThreadPool;
	CJob* m_Job;
	void *m_JobData;
	
	std::mutex m_VarMutex;
	bool m_IsEnd;
	
protected:

public:
	std::condition_variable m_JobCond;
	std::mutex m_JobCondMutex;
	
	std::mutex m_WorkMutex;
	CWorkerThread();
	virtual ~CWorkerThread();
	void Run(void);
	void SetJob(CJob *job,void *jobdata);
	CJob* GetJob(void){return m_Job;}
	void SetThreadPool(CThreadPool *thrpool);
	CThreadPool *GetThreadPool(void){return m_ThreadPool;}
};

#endif
