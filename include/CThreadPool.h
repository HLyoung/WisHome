#ifndef CTHREADPOOL_H_
#define CTHREADPOOL_H_

#include "CWorkerThread.h"
#include "CJob.h"
#include <mutex>
#include <condition_variable>
#include <vector>
#include <algorithm>
#include <assert.h>

#include <iostream>
using namespace std;

class CWorkerThread;
class CThreadPool
{

friend CWorkerThread;

private:
	std::vector<CWorkerThread *> vWork;
	unsigned int m_InitNum;
	
protected:
	void CreateIdleThread(int num);
	
public:
	
	CThreadPool();
	CThreadPool(int initnum);
	virtual ~CThreadPool();

	void SetInitNum(int initnum){m_InitNum = initnum;}
	int GetInitNum(void){return m_InitNum;}
	
	void TerminateAll(void);
	void Run(CJob *job,unsigned int index);   //�����ļ����������¼���ӵ���Ӧ���¼�������

};


#endif //CTHREADPOOL_H_
