

#include "Thread.h"
#include <iostream>

CThread::CThread()
{
	killLck = std::unique_lock<std::mutex>(killMutex,std::defer_lock);
	killedLck = std::unique_lock<std::mutex>(killedMutex,std::defer_lock);
	suspendLck = std::unique_lock<std::mutex>(suspendMutex,std::defer_lock);
}

CThread::~CThread()
{
	
}

bool CThread::Create(bool bSuspend)
{
	killVariable.notify_all();
	killedVariable.notify_all();
	suspendVariable.notify_all();
	
	std::thread t_x(Proc,this);
	t_x.detach();
	m_ID = t_x.get_id();
	
	if(bSuspend)
	{
		Suspend();
	}
	return true;	
}

bool CThread::Suspend()
{
	suspendVariable.notify_all();
	OnSuspend();
	
	return true;
}

bool CThread::Resume()
{
	return true;
}

bool CThread::Destroy(bool bWait)
{
	killVariable.notify_all();
    second_type waitTime(10);
	if(!bWait)
	{
		return true;
	}
	while(true)
	{
		usleep(IDLE_TimeAtom*1000);
		if(killedVariable.wait_for(killLck,waitTime) == std::cv_status::no_timeout)
		{
			break;
		}				
	} 
	return true;
}

void CThread::OnCreate()
{
	
}

void CThread::OnSuspend()
{
	
}
void CThread::OnResume()
{
	
}

void CThread::OnDestroy()
{
	
}

bool CThread::IsSuspend()
{
	second_type waitTime(5);
	if(std::cv_status::timeout == suspendVariable.wait_for(suspendLck,waitTime))
	{
		return false;
	}
	return true;
}

void * CThread::Proc(void *OwnerPtr)
{
	CThread *pThis = (CThread *)OwnerPtr;
	
	milliseconds_type waitTime(5);
	pThis->OnCreate();
	for( ; ; ) //main loop
	{
		if(std::cv_status::no_timeout == pThis->killVariable.wait_for(pThis->killLck,waitTime))
		{
			break;
		}
		
		if(std::cv_status::no_timeout == pThis->suspendVariable.wait_for(pThis->suspendLck,waitTime))
		{
			usleep(5);
			continue;
		}
		pThis->OnCycle();
	}
	pThis->OnDestroy();
	pThis->killVariable.notify_all();
	
	return NULL;
}


bool CThread::IsKilled()
{
	milliseconds_type waitTime(5);
	if(std::cv_status::timeout == killVariable.wait_for(killLck,waitTime))
	{
		return false;
	}
		
	return true;
}

std::thread::id CThread::GetID() 
{
	return m_ID;
}

void CThread::Sleep(UINT32 seconds,UINT32 milliseconds)
{
	sleep(seconds);
	usleep(milliseconds*SUSECOND);
}

void CThread::Idle()
{
	usleep(IDLE_TimeAtom);
}
