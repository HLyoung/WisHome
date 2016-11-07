#ifndef CTHREADPOOL_H_
#define CTHREADPOOL_H_

#include "CWorkerThread.h"
#include "CJob.h"
#include <mutex>
#include <condition_variable>
#include <vector>
#include <algorithm>
#include <assert.h>

class CWorkerThread;
class CThreadPool
{

friend CWorkerThread;

private:
	unsigned int m_MaxNum;
	unsigned int m_AvailLow;
	unsigned int m_AvailHigh;
	unsigned int m_AvailNum;
	unsigned int m_InitNum;
	
protected:
	CWorkerThread *GetIdleThread(void);
	
	void AppendToIdleList(CWorkerThread *jobthread);
	void MoveToBusyList(CWorkerThread * idlethread);
	void MoveToIdleList(CWorkerThread *busythread);
	
	void DeleteIdleThread(int num);
	void CreateIdleThread(int num);
	
public:
	std::mutex m_BusyMutex;
	std::mutex m_IdleMutex;
	std::mutex m_JobMutex;
	std::mutex m_VarMutex;
	
	std::condition_variable  m_BusyCond;
	std::mutex m_BusyCondMutex;
	std::condition_variable  m_IdleCond;
	std::mutex m_IdleCondMutex;
	std::condition_variable  m_IdleJobCond;
	std::mutex m_IdleJjobCondMutex;
	std::condition_variable  m_MaxNumCond;
	std::mutex m_MaxNumCondMutex;
	
	std::vector<CWorkerThread *> m_ThreadList;
	std::vector<CWorkerThread *> m_BusyList;
	std::vector<CWorkerThread *> m_IdleList;
	
	CThreadPool();
	CThreadPool(int initnum);
	virtual ~CThreadPool();
	
	void SetMaxNum(int maxnum){m_MaxNum = maxnum;}
	int GetMaxNum(void){return m_MaxNum;}
	void SetAvailLowNum(int minnum){m_AvailLow = minnum;}
	int GetAvailLowNum(void){return m_AvailLow;}
	void SetAvailHighNum(int highnum){m_AvailHigh = highnum;}
	int GetAvailHighNum(void){return m_AvailHigh;}
	int GetActualAvailNum(void){return m_AvailNum;}
	int GetAllNum(void){return m_ThreadList.size();}
	int GetBusyNum(void){return m_BusyList.size();}
	void SetInitNum(int initnum){m_InitNum = initnum;}
	int GetInitNum(void){return m_InitNum;}
	
	void TerminateAll(void);
	void Run(CJob *job,void *jobdata);

};


#endif //CTHREADPOOL_H_
