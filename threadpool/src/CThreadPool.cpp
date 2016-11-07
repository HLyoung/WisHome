
#include "CThreadPool.h"
CThreadPool::CThreadPool()
{
	m_MaxNum = 50;
	m_AvailLow = 5;
	m_InitNum = m_AvailNum = 10;
	m_AvailHigh = 20;
	
	m_BusyList.clear();
	m_IdleList.clear();
	
	for(int i=0;i<m_InitNum;i++){
		CWorkerThread *thr = new CWorkerThread();
		AppendToIdleList(thr);
		thr->SetThreadPool(this);
		thr->Run();
	}
}

CThreadPool::CThreadPool(int initnum)
{

    assert(initnum>0 && initnum<=30);
    m_MaxNum   = 30;
    m_AvailLow = initnum-10>0?initnum-10:3;
    m_InitNum=m_AvailNum = initnum ;  
    m_AvailHigh = initnum+10;

 
    m_BusyList.clear();
    m_IdleList.clear();
    for(int i=0;i<m_InitNum;i++){
   	 	CWorkerThread* thr = new CWorkerThread();
    	AppendToIdleList(thr);
    	thr->SetThreadPool(this);
    	//thr->Run();       //begin the thread,the thread wait for job
    }

}

CThreadPool::~CThreadPool()
{
	TerminateAll();
}
CWorkerThread *CThreadPool::GetIdleThread(void)
{
	std::unique_lock<std::mutex> lg(m_IdleCondMutex);
	while(m_IdleList.size() == 0)
		m_IdleCond.wait(lg);
	
	m_IdleMutex.lock();
	if(m_IdleList.size()>0){
		CWorkerThread *thr = (CWorkerThread *)m_IdleList.front();
		m_IdleMutex.unlock();
		return thr;
	}
	m_IdleMutex.unlock();
	
	return NULL;
}

void CThreadPool::AppendToIdleList(CWorkerThread *jobthread)
{
	m_IdleMutex.lock();
	m_IdleList.push_back(jobthread);
	m_ThreadList.push_back(jobthread);
	m_IdleMutex.unlock();
}


void CThreadPool::MoveToBusyList(CWorkerThread *idlethread)
{
	m_BusyMutex.lock();
	m_BusyList.push_back(idlethread);
	m_AvailNum --;
	m_BusyMutex.unlock();
	
	m_IdleMutex.lock();
	std::vector<CWorkerThread *>::iterator pos;
	pos = find(m_IdleList.begin(),m_IdleList.end(),idlethread);
	
	if(pos != m_IdleList.end())
		m_IdleList.erase(pos);
	m_IdleMutex.unlock();
}


void CThreadPool::MoveToIdleList(CWorkerThread *busythread)
{
	m_IdleMutex.lock();
	m_IdleList.push_back(busythread);
	m_AvailNum ++;
	m_IdleMutex.unlock();
	
	m_BusyMutex.lock();
	std::vector<CWorkerThread *>::iterator pos;
	pos = find(m_BusyList.begin(),m_BusyList.end(),busythread);
	if(pos != m_BusyList.end())
		m_BusyList.erase(pos);
	m_BusyMutex.unlock();
	
	m_IdleCond.notify_one();
	m_MaxNumCond.notify_one();
}

void CThreadPool::CreateIdleThread(int num)
{
	for(int i= 0;i<num;i++){
		CWorkerThread *thr = new CWorkerThread();
		thr->SetThreadPool(this);
		AppendToIdleList(thr);
		
		m_VarMutex.lock();
		m_AvailNum ++;
		m_VarMutex.unlock();
		thr->Run();
	}
}

void CThreadPool::DeleteIdleThread(int num)
{
	m_IdleMutex.lock();
	
	for(int i=0;i<num;i++){
		if(m_IdleList.size()>0){
			m_IdleList.erase(m_IdleList.begin());
			m_AvailNum --;
			}	
	}
	m_IdleMutex.unlock();
}

void CThreadPool::Run(CJob *job,void *jobdata)
{
	assert(job != NULL);

	std::unique_lock<std::mutex> lg(m_MaxNumCondMutex);
	if(GetBusyNum() == m_MaxNum)
		m_MaxNumCond.wait(lg);
	
	if(m_IdleList.size() < m_AvailLow){
		if(GetAllNum() + m_InitNum - m_IdleList.size() < m_MaxNum)
			CreateIdleThread(m_InitNum-m_IdleList.size());
		else
			CreateIdleThread(m_MaxNum-GetAllNum());
	}
	
	CWorkerThread  *idlethr = GetIdleThread();
	if(idlethr != NULL){
		idlethr->m_WorkMutex.lock();
		MoveToBusyList(idlethr);
		idlethr->SetThreadPool(this);
		job->SetWorkThread(idlethr);
		idlethr->SetJob(job,jobdata);
	}
}


void CThreadPool::TerminateAll()

{

    for(int i=0;i < m_ThreadList.size();i++) {

    CWorkerThread* thr = m_ThreadList[i];

    thr->Join();

    }

    return;

}


