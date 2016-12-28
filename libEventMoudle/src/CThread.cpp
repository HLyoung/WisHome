#include "CThread.h"

#include <iostream>
CThread::CThread()
{
	m_Thread = std::thread(ThreadFunction,this);
	m_ThreadID = m_Thread.get_id();
	m_Thread.detach();
}

CThread::CThread(bool detach)
{
	m_Thread = std::thread(ThreadFunction,this);
	m_ThreadID = m_Thread.get_id();
	if(detach)
		m_Thread.detach();
	else
		m_Thread.join();
}
void CThread::ThreadFunction(void* param)
{
	CThread * pThread = (CThread *)param;
	pThread->Run();
}

void CThread::Detach(void)
{
	m_Thread.detach();
	
}

void CThread::Join(void)
{
	m_Thread.join();
}

std::thread::id CThread::Self(void)
{
	return m_Thread.get_id();
}
