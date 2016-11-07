#ifndef CTHREAD_MANAGE_H_
#define CTHREAD_MANAGE_H_

#include "CThreadPool.h"

class CThreadPool;
class CThreadManage
{
private:
	CThreadPool *m_Pool;
	int m_NumOfThread;
protected:
	
public:
	void SetParalleNum(int num);
	CThreadManage();
	CThreadManage(int num);
	virtual ~CThreadManage();
	
	void Run(CJob* job,void *jobdata);
	void TerminateAll(void);
};

#endif  //CTHREAD_MANAGE_H_

