#ifndef CWORKTHREAD_H_
#define CWORKTHREAD_H_

#include <condition_variable>
#include <mutex>
#include "CThreadPool.h"
#include "CJob.h"
#include <list>


using namespace std;
class CThreadPool;
class CJob;
class CWorkerThread :public CThread
{
private:
	CThreadPool *m_ThreadPool;
	std::list<CJob*> jobList;
	std::condition_variable m_JobCond;
	std::mutex m_JobCondMutex;
	std::mutex m_VarMutex;
protected:

public:
	CWorkerThread();
	virtual ~CWorkerThread();
	void Run(void);
	void addJob(CJob *job);
	CJob* getJob(void);
	void SetThreadPool(CThreadPool *thrpool);
	CThreadPool *GetThreadPool(void){return m_ThreadPool;}
};

#endif
