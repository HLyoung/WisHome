
#include "CThreadPool.h"
CThreadPool::CThreadPool()
{
	m_InitNum =  10;
	vWork.clear();
	this->CreateIdleThread(m_InitNum);
		
}

CThreadPool::CThreadPool(int initnum)
{

    assert(initnum>0 && initnum<=30);
    m_InitNum = initnum;
    this->CreateIdleThread(m_InitNum);
}

CThreadPool::~CThreadPool()
{
	TerminateAll();
}
void CThreadPool::CreateIdleThread(int num)
{
	for(int i= 0;i<num;i++){
		CWorkerThread *thr = new CWorkerThread();
		thr->SetThreadPool(this);
		vWork.push_back(thr);
	}
}

void CThreadPool::Run(CJob *job,unsigned int  index)
{
	assert(job != NULL);
	CWorkerThread *worker = vWork[index%vWork.size()];
	worker->addJob(job);
}


void CThreadPool::TerminateAll()
{

}


